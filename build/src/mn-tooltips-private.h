/* Generated by GOB (v2.0.17)   (do not edit directly) */

#ifndef __MN_TOOLTIPS_PRIVATE_H__
#define __MN_TOOLTIPS_PRIVATE_H__

#include "mn-tooltips.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#line 32 "src/mn-tooltips.gob"

typedef struct
{
  MNTooltips	*self;
  GtkWidget	*widget;
  GtkWidget	*tip_widget;
} TooltipsData;

#line 23 "mn-tooltips-private.h"
struct _MNTooltipsPrivate {
#line 76 "src/mn-tooltips.gob"
	GtkWidget * window;
#line 77 "src/mn-tooltips.gob"
	TooltipsData * active_data;
#line 78 "src/mn-tooltips.gob"
	GSList * data_list;
#line 80 "src/mn-tooltips.gob"
	gboolean use_sticky_delay;
#line 81 "src/mn-tooltips.gob"
	GTimeVal last_popdown;
#line 82 "src/mn-tooltips.gob"
	unsigned int timeout_id;
#line 84 "src/mn-tooltips.gob"
	int border_width;
#line 39 "mn-tooltips-private.h"
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
