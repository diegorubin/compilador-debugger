/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*!
* win_main.h
* Copyright (C) Diego Rubin 2011 <rubin.diego@gmail.com>
*
* Gnomato is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Gnomato is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Author: Diego Rubin <rubin.diego@gmail.com>
*
*/

#ifndef _WIN_MAIN_H_
#define _WIN_MAIN_H_

#include <gtkmm.h>
#include <glibmm/main.h>
#include <glibmm/iochannel.h>
#include <glibmm/i18n.h>
#include <libnotify/notify.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <unistd.h> 

#include <sys/types.h>
#include <sys/stat.h>

#include "dialog_preferences.h"
#include "config.h"
#include "utils.h"

#define input "/tmp/mypas"

using namespace Gtk;

enum {
  WAITING=260,
  SYMTABCLEAR,
  SYMTABINSERT,
};

class WinMain: public Gtk::Window
{
public:
  WinMain(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~WinMain();

  void set_systray(Glib::RefPtr<Gtk::StatusIcon> tray);

protected:

private:

	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
	   ModelColumns()
	   { add(id); add(dttype); add(idtype); add(offset); }
	   Gtk::TreeModelColumn< Glib::ustring > id;
	   Gtk::TreeModelColumn< Glib::ustring > dttype;
	   Gtk::TreeModelColumn< Glib::ustring > idtype;
	   Gtk::TreeModelColumn< Glib::ustring > offset;
	};

  // attributes
  int inputfd;
  bool showed;

  /* for commands */
  bool symtab;
  int codecommand;

  Glib::RefPtr<Glib::IOChannel> inputchannel;

  Config configs;

  Glib::RefPtr<Gtk::Builder> m_refGlade;

  // widgets
  ImageMenuItem *mnuQuit;
  ImageMenuItem *mnuPreferences;
  ImageMenuItem *mnuAbout;

  TreeView *trvSymtab;
	ModelColumns mdlColumn;

  Glib::RefPtr<StatusIcon> systray;
  Glib::RefPtr<Gtk::UIManager> mnuSystray;

  Glib::RefPtr<ActionGroup> actMenu;
  Glib::RefPtr<ListStore> treSymtab;

  //dialogs
  AboutDialog *abtDialog;

  // methods
  void notify(const char *message);

  // callback methods
  virtual void on_systray_activated();
  virtual void on_systray_popup(guint button, guint activate_time);
  virtual void on_cursor_changed();
  virtual bool inputcall(Glib::IOCondition io_condition);

  // callback methods - menu
  virtual void on_menu_file_quit();
  virtual void on_menu_edit_preferences();
  virtual void on_menu_help_about();

  // aux methods
  virtual void insert_row_in_symtab(std::string data);
};

#endif //__WIN_MAIN_H_
