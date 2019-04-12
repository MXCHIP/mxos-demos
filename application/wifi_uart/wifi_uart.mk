#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := App_WiFi_UART

$(NAME)_SOURCES := main.c \
                   tcp_client.c \
                   tcp_server.c \
                   spp_protocol.c \
                   uart_recv.c \
                   para_storage.c
                   

