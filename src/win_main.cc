/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*!
* win_main.cc
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

#include "win_main.h"

char *dtypes[] = {
  "INTEGER",
  "REAL",
  "BOOLEAN",
};

char *idtypes[] = {
  "VARIABLE",
  "CONSTANT",
  "PROCEDURE",
  "FUNCTION",
  "PARAMETER",
};

WinMain::WinMain(BaseObjectType* cobject, 
                 const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade) 
{

  codecommand = WAITING;

  showed = false;
  symtab = false;

  configs.load();

  // get widgets
  m_refGlade->get_widget("trvSymtab", trvSymtab);
  m_refGlade->get_widget("txtSourcecode", txtSourcecode);

  m_refGlade->get_widget("lblStatus", lblStatus);
  lblStatus->set_text("Esperando");

  m_refGlade->get_widget("mnuQuit", mnuQuit);
  m_refGlade->get_widget("mnuPreferences", mnuPreferences);
  m_refGlade->get_widget("mnuAbout", mnuAbout);

  treSymtab = Gtk::ListStore::create(mdlColumn);
  trvSymtab->set_model(treSymtab);

  // connect signals
  trvSymtab->signal_cursor_changed().
            connect(sigc::mem_fun(*this, &WinMain::on_cursor_changed));

  // connect menu
  mnuQuit->signal_activate().
            connect(sigc::mem_fun(*this, &WinMain::on_menu_file_quit));
  mnuPreferences->signal_activate().
            connect(sigc::mem_fun(*this, &WinMain::on_menu_edit_preferences));
  mnuAbout->signal_activate().
            connect(sigc::mem_fun(*this, &WinMain::on_menu_help_about));

  inputfd = open(input, O_RDONLY);
  if (inputfd == -1)
  {
    std::cerr << "error opening fifo" << std::endl;
    //return -1;
  }
  
  // connect the signal handler
  Glib::signal_io().connect(sigc::mem_fun(+this,&WinMain::inputcall), inputfd, Glib::IO_IN);

  // Creates a iochannel from the file descriptor
  inputchannel = Glib::IOChannel::create_from_fd(inputfd);


	show_all_children();
}

WinMain::~WinMain()
{
}

void WinMain::set_systray(Glib::RefPtr<StatusIcon> tray)
{
  systray = tray;

  //criacao do menu para o systray
	actMenu = Gtk::ActionGroup::create();
  
	actMenu->add(Gtk::Action::create("Quit",Gtk::Stock::QUIT),
	             sigc::mem_fun(*this, &WinMain::on_menu_file_quit));
	
	mnuSystray = Gtk::UIManager::create();
	mnuSystray->insert_action_group(actMenu);
	
	Glib::ustring ui_info =
    "<ui>"
    "  <popup name='Popup'>"
    "    <separator/>"
    "    <menuitem action='Quit' />"
    "  </popup>"
    "</ui>";	

  mnuSystray->add_ui_from_string(ui_info);

  systray->set_tooltip_text("Cdb");
 
  systray->signal_popup_menu().
           connect(sigc::mem_fun(*this,&WinMain::on_systray_popup));
  
  systray->signal_activate().
           connect(sigc::mem_fun(*this,&WinMain::on_systray_activated));

}

// methods implementations
void WinMain::notify(const char *message)
{
  NotifyNotification *notMessage;
  GError *error = NULL;

  notMessage = notify_notification_new("Cdb",
                                       message,
                                       CDB_DATADIR "/systrayicon.png");
  
  notify_notification_set_timeout(notMessage,3000);

  char category[30] = "Cdb Notifications";
  notify_notification_set_category(notMessage,category);

  notify_notification_set_urgency(notMessage,NOTIFY_URGENCY_CRITICAL);

  notify_notification_show(notMessage,&error);
}

// callbacks implementations
void WinMain::on_systray_activated()
{
  if(showed){
    showed = false;
    hide();
  }else{
    showed = true;
    show();
  }
}

void WinMain::on_systray_popup(guint button, guint activate_time)
{
  Gtk::Menu* pMenu = static_cast<Gtk::Menu*>(mnuSystray->get_widget("/Popup"));
 
  if(pMenu)
    pMenu->popup(button, activate_time);
}

void WinMain::on_cursor_changed()
{

}

bool WinMain::inputcall(Glib::IOCondition io_condition)
{
  Glib::ustring command;
  std::string data;

  if ((io_condition & Glib::IO_IN) == 0) {
    std::cerr << "Invalid fifo response" << std::endl;
  } else {
    Glib::ustring buf;
    inputchannel->read_line(buf);
    data = buf;

    switch(codecommand) {
      case WAITING:
        codecommand = atoi(buf.c_str());
        lblStatus->set_text("Esperando");
        break;
      case SYMTABCLEAR:
        lblStatus->set_text("Apagando Symtab");
        rows.clear();
        load_symtab();
        codecommand = WAITING;
        break;
      case SYMTABINSERT:
        lblStatus->set_text("Inserindo na Symtab");
        insert_row_in_symtab(data);
        codecommand = WAITING;
        break;
      case INSERTINPUTCODE:
        lblStatus->set_text("Inserindo Sourcecode");
        insert_source(data);
        codecommand = WAITING;
        break;
      case CLEAR:
        lblStatus->set_text("Limpando");
        rows.clear();
        load_symtab();
        insert_source("");
        codecommand = WAITING;
        break;
      default:
        codecommand = atoi(buf.c_str());
    }

  }

  return true;

}

// callbacks implementations - menu
void WinMain::on_menu_file_quit()
{
  Main::quit();
}

void WinMain::on_menu_edit_preferences()
{
  DialogPreferences *dlgPreferences = 0;

  m_refGlade->get_widget_derived("DialogPreferences",dlgPreferences);
  dlgPreferences->run();

  configs.load();
}

void WinMain::on_menu_help_about()
{
  m_refGlade->get_widget("winAbout", abtDialog);
  abtDialog->run();
  abtDialog->hide();
}

// methods
void WinMain::insert_row_in_symtab(std::string data)
{
  rows.push_back(data);
  load_symtab();
}

void WinMain::load_symtab()
{
  int k = 0;
  int dtype;
  
  std::string cell;
  std::list<std::string> tmp = rows;

  std::string cols[6];

  treSymtab->clear();
  trvSymtab->remove_all_columns();

  while(!tmp.empty()){
    Gtk::TreeModel::Row row = *(treSymtab->append());

    std::stringstream lineStream(tmp.front());
    
    k = 0;
    while(std::getline(lineStream,cell,',')) {
      cols[k++] = cell;
    }

    row[mdlColumn.id] = cols[0];
    dtype = atoi(cols[1].c_str()) - INTEGER;
    if(dtype < 0)
      row[mdlColumn.dttype] = "VOID";
    else
      row[mdlColumn.dttype] = dtypes[dtype];

    row[mdlColumn.idtype] = idtypes[atoi(cols[2].c_str()) -1];
    row[mdlColumn.offset] = cols[3];

    tmp.pop_front();
  }

  trvSymtab->append_column("ID", mdlColumn.id);
  trvSymtab->append_column(_("Data Type"), mdlColumn.dttype);
  trvSymtab->append_column(_("ID Type"), mdlColumn.idtype);
  trvSymtab->append_column(_("Offset"), mdlColumn.offset);

}

void WinMain::insert_source(std::string data)
{
  tbfSource = Gtk::TextBuffer::create();

  tbfSource->set_text(data);
  txtSourcecode->set_buffer(tbfSource);
}

