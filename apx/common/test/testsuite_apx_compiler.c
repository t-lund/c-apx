/*****************************************************************************
* \file      testsuite_apx_compiler.c
* \author    Conny Gustafsson
* \date      2019-01-03
* \brief     Unit Tests for apx_compiler
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
#include <stdio.h>
#include <stddef.h>
#include "CuTest.h"
#include "apx_compiler.h"
#include "apx_parser.h"
#include "apx_vmdefs.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_apx_compiler_encodeInstruction_packU8(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packU16(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packU32(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packU64(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packS8(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packS16(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packS32(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packS64(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packBool(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packBytes(CuTest* tc);
static void test_apx_compiler_encodeInstruction_packStr(CuTest* tc);
static void test_apx_compiler_compilePackDataElement_U8(CuTest* tc);
static void test_apx_compiler_compilePackDataElement_U8FixArrayU8(CuTest* tc);
static void test_apx_compiler_compilePackDataElement_U8FixArrayU16(CuTest* tc);
static void test_apx_compiler_compilePackDataElement_U8FixArrayU32(CuTest* tc);
static void test_apx_compiler_compilePackDataElement_U8DynArrayU8(CuTest* tc);


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testSuite_apx_compiler(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packU8);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packU16);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packU32);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packU64);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packS8);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packS16);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packS32);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packS64);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packBool);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packBytes);
   SUITE_ADD_TEST(suite, test_apx_compiler_encodeInstruction_packStr);
   SUITE_ADD_TEST(suite, test_apx_compiler_compilePackDataElement_U8);
   SUITE_ADD_TEST(suite, test_apx_compiler_compilePackDataElement_U8FixArrayU8);
   SUITE_ADD_TEST(suite, test_apx_compiler_compilePackDataElement_U8FixArrayU16);
   SUITE_ADD_TEST(suite, test_apx_compiler_compilePackDataElement_U8FixArrayU32);
   SUITE_ADD_TEST(suite, test_apx_compiler_compilePackDataElement_U8DynArrayU8);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
#if 0 //THis is for when opcode is 2 bits
static void test_apx_compiler_encodeInstruction_packU8(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00000001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U8, 0u) );

   CuAssertUIntEquals(tc, 0b10000001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U8, APX_INST_FLAG) );

}

static void test_apx_compiler_encodeInstruction_packU16(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00000101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U16, 0u) );

   CuAssertUIntEquals(tc, 0b10000101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U16, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packU32(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00001001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U32, 0u) );

   CuAssertUIntEquals(tc, 0b10001001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U32, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packU64(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00001101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U64, 0u) );

   CuAssertUIntEquals(tc, 0b10001101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U64, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS8(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S8, 0u) );

   CuAssertUIntEquals(tc, 0b10010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S8, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS16(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00010101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S16, 0u) );

   CuAssertUIntEquals(tc, 0b10010101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S16, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS32(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S32, 0u) );

   CuAssertUIntEquals(tc, 0b10011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S32, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS64(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00011101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S64, 0u) );

   CuAssertUIntEquals(tc, 0b10011101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S64, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packBool(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00101001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BOOL, 0u) );

   CuAssertUIntEquals(tc, 0b10101001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BOOL, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packBytes(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00101101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BYTES, 0u) );

   CuAssertUIntEquals(tc, 0b10101101, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BYTES, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packStr(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00110001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_STR, 0u) );

   CuAssertUIntEquals(tc, 0b10110001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_STR, APX_INST_FLAG) );
}
#endif

#if 1 //THis is for when opcode is 3 bits
static void test_apx_compiler_encodeInstruction_packU8(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00000001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U8, 0u) );

   CuAssertUIntEquals(tc, 0b10000001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U8, APX_INST_FLAG) );

}

static void test_apx_compiler_encodeInstruction_packU16(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00001001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U16, 0u) );

   CuAssertUIntEquals(tc, 0b10001001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U16, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packU32(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U32, 0u) );

   CuAssertUIntEquals(tc, 0b10010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U32, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packU64(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U64, 0u) );

   CuAssertUIntEquals(tc, 0b10011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_U64, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS8(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00100001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S8, 0u) );

   CuAssertUIntEquals(tc, 0b10100001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S8, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS16(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00101001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S16, 0u) );

   CuAssertUIntEquals(tc, 0b10101001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S16, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS32(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00110001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S32, 0u) );

   CuAssertUIntEquals(tc, 0b10110001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S32, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packS64(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b00111001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S64, 0u) );

   CuAssertUIntEquals(tc, 0b10111001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_S64, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packBool(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b01010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BOOL, 0u) );

   CuAssertUIntEquals(tc, 0b11010001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BOOL, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packBytes(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b01011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BYTES, 0u) );

   CuAssertUIntEquals(tc, 0b11011001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_BYTES, APX_INST_FLAG) );
}

static void test_apx_compiler_encodeInstruction_packStr(CuTest* tc)
{
   CuAssertUIntEquals(tc, 0b01100001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_STR, 0u) );

   CuAssertUIntEquals(tc, 0b11100001, apx_compiler_encodeInstruction(APX_OPCODE_PACK, APX_VARIANT_STR, APX_INST_FLAG) );
}
#endif

static void test_apx_compiler_compilePackDataElement_U8(CuTest* tc)
{
   uint8_t opcode, variant, flags;
   adt_bytearray_t *program = adt_bytearray_new(APX_PROGRAM_GROW_SIZE);
   apx_dataElement_t *element = apx_dataElement_new(APX_BASE_TYPE_UINT8, NULL);
   apx_compiler_t *compiler = apx_compiler_new();
   CuAssertPtrNotNull(tc, compiler);

   apx_compiler_setProgram(compiler, program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_compilePackDataElement(compiler, element));
   CuAssertIntEquals(tc, 1, adt_bytearray_length(program));
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(adt_bytearray_data(program)[0], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_PACK, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, 0, flags);

   apx_compiler_delete(compiler);
   apx_dataElement_delete(element);
   adt_bytearray_delete(program);
}

static void test_apx_compiler_compilePackDataElement_U8FixArrayU8(CuTest* tc)
{
   uint8_t opcode, variant, flags;
   adt_bytearray_t *program = adt_bytearray_new(APX_PROGRAM_GROW_SIZE);
   apx_dataElement_t *element = apx_dataElement_new(APX_BASE_TYPE_UINT8, NULL);
   apx_compiler_t *compiler = apx_compiler_new();
   uint8_t *code;
   CuAssertPtrNotNull(tc, compiler);
   apx_dataElement_setArrayLen(element, 32);

   apx_compiler_setProgram(compiler, program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_compilePackDataElement(compiler, element));
   CuAssertIntEquals(tc, 3, adt_bytearray_length(program));
   code = adt_bytearray_data(program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[0], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_PACK, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, APX_ARRAY_FLAG, flags);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[1], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_ARRAY, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, 0, flags);
   CuAssertUIntEquals(tc, apx_dataElement_getArrayLen(element), code[2]);

   apx_compiler_delete(compiler);
   apx_dataElement_delete(element);
   adt_bytearray_delete(program);
}

static void test_apx_compiler_compilePackDataElement_U8FixArrayU16(CuTest* tc)
{
   uint8_t opcode, variant, flags;
   adt_bytearray_t *program = adt_bytearray_new(APX_PROGRAM_GROW_SIZE);
   apx_dataElement_t *element = apx_dataElement_new(APX_BASE_TYPE_UINT8, NULL);
   apx_compiler_t *compiler = apx_compiler_new();
   uint8_t *code;
   CuAssertPtrNotNull(tc, compiler);
   apx_dataElement_setArrayLen(element, 4095);

   apx_compiler_setProgram(compiler, program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_compilePackDataElement(compiler, element));
   CuAssertIntEquals(tc, 4, adt_bytearray_length(program));
   code = adt_bytearray_data(program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[0], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_PACK, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, APX_ARRAY_FLAG, flags);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[1], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_ARRAY, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U16, variant);
   CuAssertUIntEquals(tc, 0, flags);
   CuAssertUIntEquals(tc, 0xff, code[2]);
   CuAssertUIntEquals(tc, 0x0f, code[3]);

   apx_compiler_delete(compiler);
   apx_dataElement_delete(element);
   adt_bytearray_delete(program);
}

static void test_apx_compiler_compilePackDataElement_U8DynArrayU8(CuTest* tc)
{
   uint8_t opcode, variant, flags;
   adt_bytearray_t *program = adt_bytearray_new(APX_PROGRAM_GROW_SIZE);
   apx_dataElement_t *element = apx_dataElement_new(APX_BASE_TYPE_UINT8, NULL);
   apx_compiler_t *compiler = apx_compiler_new();
   uint8_t *code;
   CuAssertPtrNotNull(tc, compiler);
   apx_dataElement_setArrayLen(element, 32);
   apx_dataElement_setDynamicArray(element);

   apx_compiler_setProgram(compiler, program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_compilePackDataElement(compiler, element));
   CuAssertIntEquals(tc, 2, adt_bytearray_length(program));
   code = adt_bytearray_data(program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[0], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_PACK, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, APX_ARRAY_FLAG, flags);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[1], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_ARRAY, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, APX_DYN_ARRAY_FLAG, flags);

   apx_compiler_delete(compiler);
   apx_dataElement_delete(element);
   adt_bytearray_delete(program);
}

static void test_apx_compiler_compilePackDataElement_U8FixArrayU32(CuTest* tc)
{
   uint8_t opcode, variant, flags;
   adt_bytearray_t *program = adt_bytearray_new(APX_PROGRAM_GROW_SIZE);
   apx_dataElement_t *element = apx_dataElement_new(APX_BASE_TYPE_UINT8, NULL);
   apx_compiler_t *compiler = apx_compiler_new();
   uint8_t *code;
   CuAssertPtrNotNull(tc, compiler);
   apx_dataElement_setArrayLen(element, 95000);

   apx_compiler_setProgram(compiler, program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_compilePackDataElement(compiler, element));
   CuAssertIntEquals(tc, 6, adt_bytearray_length(program));
   code = adt_bytearray_data(program);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[0], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_PACK, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U8, variant);
   CuAssertUIntEquals(tc, APX_ARRAY_FLAG, flags);
   CuAssertIntEquals(tc, APX_NO_ERROR, apx_compiler_decodeInstruction(code[1], &opcode, &variant, &flags));
   CuAssertUIntEquals(tc, APX_OPCODE_ARRAY, opcode);
   CuAssertUIntEquals(tc, APX_VARIANT_U32, variant);
   CuAssertUIntEquals(tc, 0, flags);
   CuAssertUIntEquals(tc, 0x18, code[2]);
   CuAssertUIntEquals(tc, 0x73, code[3]);
   CuAssertUIntEquals(tc, 0x01, code[4]);
   CuAssertUIntEquals(tc, 0x00, code[5]);

   apx_compiler_delete(compiler);
   apx_dataElement_delete(element);
   adt_bytearray_delete(program);
}
