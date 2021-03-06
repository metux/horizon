#pragma once
#include <gtkmm.h>
#include <memory>
#include "uuid.hpp"
#include "sort_controller.hpp"
#include "selection_provider.hpp"

namespace horizon {
	class PoolBrowser: public Gtk::Box, public SelectionProvider {
		public:
			PoolBrowser(class Pool *pool);
			UUID get_selected() override;
			void set_show_none(bool v);
			void set_show_path(bool v);
			virtual void search() = 0;

		protected:
			void construct();
			class Pool *pool = nullptr;
			bool show_none = false;
			bool show_path = false;
			int path_column = -1;


			Gtk::TreeView *treeview = nullptr;

			Gtk::Entry *create_search_entry(const std::string &label);
			void add_search_widget(const std::string &label, Gtk::Widget &w);



			virtual Glib::RefPtr<Gtk::ListStore> create_list_store() = 0;
			virtual void create_columns() = 0;
			virtual void add_sort_controller_columns() = 0;
			virtual UUID uuid_from_row(const Gtk::TreeModel::Row &row) = 0;

			Glib::RefPtr<Gtk::ListStore> store;
			std::unique_ptr<SortController> sort_controller;

			void row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
			void selection_changed();

			void select_uuid(const UUID &uu);
			void scroll_to_selection();
		private :
			Gtk::Grid *grid = nullptr;
			int grid_top = 0;
	};
}
