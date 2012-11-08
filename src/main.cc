/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*!
* main.cc
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

#include <gtkmm.h>
#include <giomm.h>
#include <glibmm.h>

#include <glibmm/i18n.h>
#include <libnotify/notify.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <iostream>

#include "utils.h"
#include "win_main.h"


char dbpath[1000];
WinMain* winMain;

int main(int argc, char **argv)
{

  //intltool
  bindtextdomain(GETTEXT_PACKAGE, CDB_LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
  
  Gtk::Main kit(argc, argv);

  notify_init("Cdb");

  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
  try
  {
    refBuilder->add_from_file(CDB_DATADIR "/cdb.ui");
  }
  catch(const Glib::FileError& ex)
  {
    std::cerr << "FileError: " << ex.what() << std::endl;
    return 1;
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }

  Glib::RefPtr<Gtk::StatusIcon> systray = 
    Gtk::StatusIcon::create_from_file(CDB_DATADIR "/systrayicon.png");

  winMain = 0;
  refBuilder->get_widget_derived("winMain", winMain);

  Gio::init();

  if(winMain)
  {
    winMain->set_icon_from_file(CDB_DATADIR "/systrayicon.png");
    winMain->set_systray(systray);
    winMain->show();
    kit.run();
  }

  delete winMain;

  return 0;
}

