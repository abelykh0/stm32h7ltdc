#ifndef __USBD_VIDEO_CONF__H__
#define __USBD_VIDEO_CONF__H__

#define UVC_WIDTH                   512U
#define UVC_HEIGHT                  256U
#define UVC_CAM_FPS_FS              30U
#define PACKET_SIZE_NO_HEADER       864U

#define UVC_MAX_FRAME_SIZE          (UVC_WIDTH * UVC_HEIGHT * 3U / 2U) // nv12
#define UVC_ISO_FS_MPS              ((unsigned int)(PACKET_SIZE_NO_HEADER + 2))
#define PACKETS_IN_FRAME            (UVC_MAX_FRAME_SIZE / PACKET_SIZE_NO_HEADER)

#endif
