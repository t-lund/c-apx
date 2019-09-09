/*****************************************************************************
* \file      apx_serverSocketExtension.h
* \author    Conny Gustafsson
* \date      2019-09-04
* \brief     APX socket server extension (TCP+UNIX)
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
#ifndef APX_SERVER_SOCKET_EXTENSION_H
#define APX_SERVER_SOCKET_EXTENSION_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "dtl_type.h"
#include "testsocket.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
//Forward declaration
struct apx_server_tag;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
apx_error_t apx_serverSocketExtension_init(struct apx_server_tag *apx_server, dtl_dv_t *config);
void apx_serverSocketExtension_shutdown(void);
//apx_error_t apx_serverSocketExtension_start(void);
//void apx_serverSocketExtension_stop(void);

apx_error_t apx_serverSocketExtension_register(struct apx_server_tag *apx_server, dtl_dv_t *config);

#ifdef UNIT_TEST
void apx_serverSocketExtension_acceptTestSocket(testsocket_t *sock);
#endif

#endif //APX_SERVER_SOCKET_EXTENSION_H