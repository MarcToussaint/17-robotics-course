/*  ------------------------------------------------------------------
    Copyright 2016 Marc Toussaint
    email: marc.toussaint@informatik.uni-stuttgart.de
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version. This program is distributed without
    any warranty. See the GNU General Public License for more details.
    You should have received a COPYING file of the full GNU General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>
    --------------------------------------------------------------  */


#ifndef MLR_gtk_h
#define MLR_gtk_h

#include <Core/array.h>

typedef struct _GtkWidget GtkWidget;

void gtkLock(bool checkInitialized=true);
void gtkUnlock();
void gtkCheckInitialized();
void gtkEnterCallback();
void gtkLeaveCallback();

int gtkPopupMenuChoice(StringL& choices);
GtkWidget *gtkTopWindow(const char* title);

void gtkProcessEvents();

#endif
