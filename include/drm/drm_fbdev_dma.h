/* SPDX-License-Identifier: MIT */

#ifndef DRM_FBDEV_DMA_H
#define DRM_FBDEV_DMA_H

struct drm_fb_helper;
struct drm_fb_helper_surface_size;

#ifdef CONFIG_DRM_FBDEV_EMULATION
int drm_fbdev_dma_driver_fbdev_probe(struct drm_fb_helper *fb_helper,
				     struct drm_fb_helper_surface_size *sizes);

#define DRM_FBDEV_DMA_DRIVER_OPS \
	.fbdev_probe = drm_fbdev_dma_driver_fbdev_probe
#else
#define DRM_FBDEV_DMA_DRIVER_OPS \
	.fbdev_probe = NULL
#endif

#endif