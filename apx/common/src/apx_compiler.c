/*****************************************************************************
* \file      apx_compiler.c
* \author    Conny Gustafsson
* \date      2019-01-03
* \brief     APX bytecode compiler
*
* Copyright (c) 2019 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "apx_compiler.h"
#include "apx_vmdefs.h"
#include "pack.h"
#include <malloc.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void apx_compiler_appendPlaceHolderHeader(apx_compiler_t *self, uint8_t progType);
static uint8_t apx_compiler_encodePackArrayInstruction(apx_compiler_t *self,  uint32_t arrayLen, bool isDynamic, uint8_t *packLen);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void apx_compiler_create(apx_compiler_t *self)
{
   if (self != 0)
   {
      self->program = (adt_bytearray_t*) 0;
      self->minDataSize = 0;
   }
}

void apx_compiler_destroy(apx_compiler_t *self)
{
   //nothing to do (yet)
}

apx_compiler_t* apx_compiler_new(void)
{
   apx_compiler_t *self = (apx_compiler_t*) malloc(sizeof(apx_compiler_t));
   if (self != 0)
   {
      apx_compiler_create(self);
   }
   return self;
}

void apx_compiler_delete(apx_compiler_t *self)
{
   if (self != 0)
   {
      apx_compiler_destroy(self);
      free(self);
   }
}

void apx_compiler_setProgram(apx_compiler_t *self, adt_bytearray_t *program)
{
   if ( (self != 0) && (program != 0) )
   {
      self->program = program;
   }
}


apx_error_t apx_compiler_compilePackDataElement(apx_compiler_t *self, apx_dataElement_t *dataElement)
{
   if ( (self != 0) && (dataElement != 0) )
   {
      apx_error_t retval = APX_NO_ERROR;
      uint8_t opcode = APX_OPCODE_PACK;
      uint8_t variant = 0;
      uint8_t flags = 0;
      apx_size_t elemSize = 0;

      if (self->program == 0)
      {
         return APX_MISSING_BUFFER_ERROR;
      }

      if (dataElement->arrayLen > 0)
      {
         flags |= APX_ARRAY_FLAG;
      }
      switch(dataElement->baseType)
      {
      case APX_BASE_TYPE_NONE:
         retval = APX_ELEMENT_TYPE_ERROR;
         break;
      case APX_BASE_TYPE_UINT8:
         variant = APX_VARIANT_U8;
         elemSize = UINT8_SIZE;
         break;
      case APX_BASE_TYPE_UINT16:
         variant = APX_VARIANT_U16;
         elemSize = UINT16_SIZE;
         break;
      case APX_BASE_TYPE_UINT32:
         variant = APX_VARIANT_U16;
         elemSize = UINT32_SIZE;
         break;
      case APX_BASE_TYPE_SINT8:
         variant = APX_VARIANT_S8;
         elemSize = UINT8_SIZE;
         break;
      case APX_BASE_TYPE_SINT16:
         variant = APX_VARIANT_S16;
         elemSize = UINT16_SIZE;
         break;
      case APX_BASE_TYPE_SINT32:
         variant = APX_VARIANT_S16;
         elemSize = UINT32_SIZE;
         break;
      default:
         retval = APX_ELEMENT_TYPE_ERROR;
         break;
      }
      if ( elemSize > 0)
      {
         uint8_t instruction = apx_compiler_encodeInstruction(opcode, variant, flags);
         adt_bytearray_push(self->program, instruction);
         if (dataElement->arrayLen > 0)
         {
            uint8_t packLen = 0u;
            uint32_t arrayLen;
            bool isDynamicArray;
            arrayLen = apx_dataElement_getArrayLen(dataElement);
            isDynamicArray = apx_dataElement_isDynamicArray(dataElement);
            instruction = apx_compiler_encodePackArrayInstruction(self, arrayLen, isDynamicArray, &packLen);
            if (instruction != APX_OPCODE_INVALID)
            {
               adt_bytearray_push(self->program, instruction);
               if ( !isDynamicArray )
               {
                  uint8_t tmp[UINT32_SIZE];
                  assert(packLen<=UINT32_SIZE);
                  packLE(&tmp[0], arrayLen, packLen);
                  adt_bytearray_append(self->program, &tmp[0], (uint32_t) packLen);
                  self->minDataSize += (elemSize*dataElement->arrayLen);
               }
            }
            else
            {
               return APX_LENGTH_ERROR;
            }
         }
         else
         {
            self->minDataSize += elemSize;
         }

      }
      return retval;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_error_t apx_compiler_compileRequirePort(apx_compiler_t *self, apx_node_t *node, apx_portId_t portId, apx_program_t *program)
{
   if ( (self != 0) && (node != 0) && (program != 0) && (portId>=0) )
   {
      int32_t numPorts = apx_node_getNumRequirePorts(node);
      if (portId >= numPorts)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      apx_compiler_appendPlaceHolderHeader(self, APX_HEADER_UNPACK_PROG);
      self->program = program;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_error_t apx_compiler_compileProvidePort(apx_compiler_t *self, apx_node_t *node, apx_portId_t portId, apx_program_t *program)
{
   if ( (self != 0) && (node != 0) && (program != 0) && (portId>=0) )
   {
      apx_error_t retval = APX_NO_ERROR;
      apx_dataElement_t *dataElement;
      apx_port_t *port;
      int32_t numPorts = apx_node_getNumProvidePorts(node);
      apx_compiler_setProgram(self, program);
      if (portId >= numPorts)
      {
         retval = APX_INVALID_ARGUMENT_ERROR;
      }
      else
      {
         apx_compiler_appendPlaceHolderHeader(self, APX_HEADER_PACK_PROG);
         port = apx_node_getProvidePort(node, portId);
         if (port != 0)
         {
            dataElement = apx_dataSignature_getDerivedDataElement(&port->dataSignature);
            if (dataElement != 0)
            {
               retval = apx_compiler_compilePackDataElement(self, dataElement);
            }
         }
         return retval;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

uint8_t apx_compiler_encodeInstruction(uint8_t opcode, uint8_t variant, uint8_t flags)
{
   uint8_t result = (opcode & APX_INST_OPCODE_MASK) | ( (variant & APX_INST_VARIANT_MASK) << APX_INST_VARIANT_SHIFT);
   if (flags != 0)
   {
      result |= (flags & APX_INST_FLAG_MASK) << APX_INST_FLAG_SHIFT;
   }
   return result;
}

apx_error_t apx_compiler_decodeInstruction(uint8_t instruction, uint8_t *opcode, uint8_t *variant, uint8_t *flags)
{
   if ( (opcode != 0) && (variant != 0) && (flags != 0) )
   {
      *opcode = instruction & APX_INST_OPCODE_MASK;
      *variant = (instruction >> APX_INST_VARIANT_SHIFT) & APX_INST_VARIANT_MASK;
      *flags = (instruction >> APX_INST_FLAG_SHIFT) & APX_INST_FLAG_MASK;
      return APX_NO_ERROR;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void apx_compiler_appendPlaceHolderHeader(apx_compiler_t *self, uint8_t progType)
{
   if ( (self->program != 0) && ((progType == APX_HEADER_UNPACK_PROG) || (progType == APX_HEADER_PACK_PROG)) )
   {
      uint8_t instruction[APX_HEADER_SIZE] = {0, 0, 0, 0, 0, 0, 0};
      packLE(&instruction[0], APX_VM_VERSION, sizeof(uint16_t));
      instruction[2] = progType;
      adt_bytearray_append(self->program, &instruction[0], (uint32_t) APX_HEADER_SIZE);
   }
}

static uint8_t apx_compiler_encodePackArrayInstruction(apx_compiler_t *self,  uint32_t arrayLen, bool isDynamic, uint8_t *packLen)
{
   const uint8_t opcode = APX_OPCODE_ARRAY;
   uint8_t variant;
   uint8_t flag = isDynamic? APX_DYN_ARRAY_FLAG : 0;
   if (arrayLen < UINT8_MAX)
   {
      variant = APX_VARIANT_U8;
      *packLen = UINT8_SIZE;
   }
   else if (arrayLen < UINT16_MAX)
   {
      variant = APX_VARIANT_U16;
      *packLen = UINT16_SIZE;
   }
   else if (arrayLen < UINT32_MAX)
   {
      variant = APX_VARIANT_U32;
      *packLen = UINT32_SIZE;
   }
   else
   {
      return APX_OPCODE_INVALID;
   }
   return apx_compiler_encodeInstruction(opcode, variant, flag);
}


