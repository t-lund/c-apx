//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "apx_fileMap.h"
#include "rmf.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif



//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define PORT_DATA_START      0x0u
#define PORT_DATA_BOUNDARY   0x400u //1KB, this must be a power of 2
#define DEFINITION_START     0x4000000 //64MB, this must be a power of 2
#define DEFINITION_BOUNDARY  0x100000u //1MB, this must be a power of 2
#define USER_DATA_START      0x20000000 //512MB, this must be a power of 2
#define USER_DATA_END        0x3FFFFC00 //Start of remote file cmd message area
#define USER_DATA_BOUNDARY   0x100000u //1MB, this must be a power of 2

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static int8_t apx_fileMap_autoInsertFile(apx_fileMap_t *self, apx_file_t *pFile, uint32_t start_address, uint32_t end_address, uint32_t address_boundary);
static int8_t apx_fileMap_insertFileInternal(apx_fileMap_t *self, apx_file_t *pFile);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void apx_fileMap_create(apx_fileMap_t *self)
{
   if (self != 0)
   {
      adt_list_create(&self->fileList, apx_file_vdelete);
      self->lastFile = (apx_file_t*) 0;
   }
}
void apx_fileMap_destroy(apx_fileMap_t *self)
{
   if (self !=0)
   {
      adt_list_destroy(&self->fileList);
   }
}

int8_t apx_fileMap_insertFile(apx_fileMap_t *self, apx_file_t *pFile)
{
   if ( (self != 0) && (pFile != 0) )
   {
      if (pFile->fileInfo.addressWithoutFlags == RMF_INVALID_ADDRESS)
      {
         //attempt to automatically assign an address to the new file
         switch(pFile->fileType)
         {
         case APX_UNKNOWN_FILE_TYPE:
            return -1;
         case APX_OUTDATA_FILE_TYPE: //intentional fall-trough
         case APX_INDATA_FILE_TYPE:
            return apx_fileMap_autoInsertFile(self, pFile, PORT_DATA_START, DEFINITION_START, PORT_DATA_BOUNDARY);
         case APX_DEFINITION_FILE_TYPE:
            return apx_fileMap_autoInsertFile(self, pFile, DEFINITION_START, USER_DATA_START, DEFINITION_BOUNDARY);
         case APX_CUSTOM_FILE_TYPE_BEGIN:
            return apx_fileMap_autoInsertFile(self, pFile, USER_DATA_START, USER_DATA_END, USER_DATA_BOUNDARY);
         }
      }
      else
      {
         return apx_fileMap_insertFileInternal(self, pFile);
      }
   }
   errno = EINVAL;
   return -1;
}


/**
 * removes pFile from self->fileList
 */
int8_t apx_fileMap_removeFile(apx_fileMap_t *self, apx_file_t *pFile)
{
   if ( (self != 0) && (pFile != 0) )
   {
      //TODO: fix adt_list_remove so that it returns success/failure
      self->lastFile = (apx_file_t*) 0;
      adt_list_remove(&self->fileList, pFile);
      return 0;
   }
   return -1;
}

apx_file_t *apx_fileMap_findByAddress(apx_fileMap_t *self, uint32_t address)
{
   apx_file_t *retval=0;
   if (self != 0)
   {
      if (self->lastFile != 0)
      {
         uint32_t startAddress;
         uint32_t endAddress;
         startAddress = self->lastFile->fileInfo.address & RMF_ADDRESS_MASK_INTERNAL;
         endAddress = startAddress + self->lastFile->fileInfo.length;
         if ( (address >= startAddress) && (address < endAddress) )
         {
            retval = self->lastFile;
         }
      }
      if (retval == 0)
      {
         adt_list_elem_t *pIter = adt_list_iter_first(&self->fileList);
         while(pIter != 0)
         {
            uint32_t startAddress;
            uint32_t endAddress;
            apx_file_t *pFile = (apx_file_t*) pIter->pItem;
            assert(pFile != 0);
            startAddress = pFile->fileInfo.address & RMF_ADDRESS_MASK_INTERNAL;
            endAddress = startAddress + pFile->fileInfo.length;
            if ( (address>=startAddress) && (address<endAddress) )
            {
               retval = pFile;
               break;
            }
            pIter = adt_list_iter_next(pIter);
         }
         if (retval != 0)
         {
            self->lastFile = retval;
         }
      }
   }
   return retval;
}

apx_file_t *apx_fileMap_findByName(apx_fileMap_t *self, const char *name)
{
   apx_file_t *retval=0;
   if (self != 0)
   {
      adt_list_elem_t *pIter = adt_list_iter_first(&self->fileList);
      while(pIter != 0)
      {
         apx_file_t *pFile = (apx_file_t*) pIter->pItem;
         assert(pFile != 0);
         if ( (strcmp(pFile->fileInfo.name, name)==0) )
         {
            retval = pFile;
            break;
         }
         pIter = adt_list_iter_next(pIter);
      }
   }
   return retval;
}

int32_t apx_fileMap_length(const apx_fileMap_t *self)
{
   if (self != 0)
   {
      return adt_list_length((adt_list_t*) &self->fileList);
   }
   return -1;
}

adt_list_t *apx_fileMap_getList(apx_fileMap_t *self)
{
   if (self != 0)
   {
      return &self->fileList;
   }
   return (adt_list_t*) 0;
}

/**
 * clears internal list of files without deleting them.
 * The caller of this function must have taken ownership of internal file objects before calling this function.
 */
void  apx_fileMap_clear_weak(apx_fileMap_t *self)
{
   if (self != 0)
   {
      adt_list_destructor_enable(&self->fileList, false);
      adt_list_clear(&self->fileList);
      adt_list_destructor_enable(&self->fileList, true);
   }
}

bool apx_fileMap_exist(apx_fileMap_t *self, apx_file_t *file)
{
   if (self != 0)
   {
      adt_list_elem_t *pIter = adt_list_iter_first(&self->fileList);
      while(pIter != 0)
      {
         if (file == (apx_file_t*) pIter->pItem)
         {
            return true;
         }
         pIter = adt_list_iter_next(pIter);
      }
   }
   return false;
}

adt_ary_t *apx_fileMap_makeFileInfoArray(apx_fileMap_t *self)
{
   if (self != 0)
   {
      adt_ary_t *array = adt_ary_new(apx_fileInfo_vdelete);
      if (array != 0)
      {
         adt_list_elem_t *pIter = adt_list_iter_first(&self->fileList);
         while(pIter != 0)
         {
            apx_fileInfo_t *fileInfo;
            apx_file_t *file = (apx_file_t*) pIter->pItem;
            fileInfo = apx_fileInfo_clone(&file->fileInfo);
            if (fileInfo == 0)
            {
               adt_ary_delete(array);
               array = (adt_ary_t*) 0;
               break;
            }
            adt_ary_push(array, fileInfo);
            pIter = adt_list_iter_next(pIter);
         }
      }
      return array;
   }
   return (adt_ary_t*) 0;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

/**
 * attempts to find the the last file in the range start_address..end_address and then inserts pFile after it
 */
static int8_t apx_fileMap_autoInsertFile(apx_fileMap_t *self, apx_file_t *pFile, uint32_t start_address, uint32_t end_address, uint32_t address_boundary)
{
   if (adt_list_is_empty(&self->fileList))
   {
      apx_fileInfo_setAddress(&pFile->fileInfo, start_address);
   }
   else
   {
      adt_list_elem_t *pIter=0;
      adt_list_elem_t *pFound=0;
      uint32_t placement_address = start_address;
      pIter = adt_list_iter_first(&self->fileList);
      while(pIter != 0)
      {
         apx_file_t *pOther;
         pOther = (apx_file_t*) pIter->pItem;
         assert(pOther != 0);

         if( pOther->fileInfo.addressWithoutFlags >= end_address)
         {
            break; //we have passed into the next area, break and use latest seen value of pFound
         }
         else if(pOther->fileInfo.addressWithoutFlags >= start_address)
         {
            pFound = pIter; //update pFound
         }
         else
         {
            //MISRA
         }
         pIter = adt_list_iter_next(pIter);
      }
      if (pFound != 0)
      {
         uint32_t other_end_address;
         uint32_t other_start_address;
         apx_file_t *pOther = (apx_file_t*) pFound->pItem;
         assert(pOther != 0);
         other_start_address=pOther->fileInfo.addressWithoutFlags;
         other_end_address=other_start_address + pOther->fileInfo.length;
         //check if address_boundary is a power of two. If not, we need to use another slower method to calculate new placement_address
         assert(address_boundary != 0);
         if ((address_boundary & (address_boundary-1)) == 0)
         {
            //address_boundary is a power of 2, use faster method
            placement_address  = (other_end_address + (address_boundary-1)) & (~(address_boundary-1)); //note that address_boundary must be a power of 2 for this code to work
         }
         else
         {
            //use slower method
            assert(0); ///TODO: implement this
         }

         if (placement_address >= end_address)
         {
            //memory map full, cannot fit any more files into this region
            errno = ENOMEM;
            return -1;
         }
      }
      apx_fileInfo_setAddress(&pFile->fileInfo, placement_address);
   }
   assert(pFile->fileInfo.addressWithoutFlags<RMF_CMD_START_ADDR);
   return apx_fileMap_insertFileInternal(self, pFile);
}

static int8_t apx_fileMap_insertFileInternal(apx_fileMap_t *self, apx_file_t *pFile)
{
   self->lastFile = (apx_file_t*) 0;
   if (adt_list_is_empty(&self->fileList))
   {
      adt_list_insert(&self->fileList, pFile);
   }
   else
   {
      apx_file_t *pLast=0;
      apx_file_t *pCurrent=0;
      adt_list_elem_t *pIter = adt_list_iter_first(&self->fileList);
      while(pIter != 0)
      {
         pCurrent = (apx_file_t*) pIter->pItem;
         assert(pCurrent != 0);
         if( pCurrent->fileInfo.addressWithoutFlags > pFile->fileInfo.addressWithoutFlags)
         {
            //try to fit pFile in before pCurrent;
            break;
         }
         pLast = pCurrent;
         pIter = adt_list_iter_next(pIter);
      }
      if (pIter != 0)
      {
         uint32_t start_address;
         uint32_t end_address;
         assert(pCurrent != 0);
         assert( ((apx_file_t*) pIter->pItem) == pCurrent);
         if (pLast != 0)
         {
            //check if there is room to fit this file between pLast and pFile
            uint32_t start_address;
            uint32_t end_address;
            start_address = pLast->fileInfo.addressWithoutFlags;
            end_address = start_address+pLast->fileInfo.length;
            if (end_address > pFile->fileInfo.addressWithoutFlags)
            {
               //address collision between pLast and pFile, reject insertion of pFile
               errno = EADDRINUSE; /* Address already in use */
               return -1;
            }
         }
         start_address = pFile->fileInfo.addressWithoutFlags;
         end_address = start_address+pFile->fileInfo.length;
         if (end_address > pCurrent->fileInfo.addressWithoutFlags)
         {
            //address collision between pCurrent and pFile, reject insertion of pFile
            errno = EFBIG; /* File too large */
            return -1;
         }
         //it should now be safe to insert the file before pIter
         adt_list_insert_before(&self->fileList, pIter, pFile);
      }
      else
      {
         //insert pFile at end of list
         adt_list_insert(&self->fileList, pFile);
      }
   }
   return 0;
}
