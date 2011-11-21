import unittest

from argonautics.ui import UI
from argonautics.test.support.mock_builder import *
from argonautics.test.support.mock_file_manager import *
from argonautics.test.support.mock_file_selection import *

from gi.repository import Gtk

class TestUI(unittest.TestCase):
  def setUp(self):
    self._mock_builder = MockBuilder(self)
    mock_file_manager_factory = MockFileManager(self).factory
    mock_file_selection_factory = MockFileSelection(self).factory

    self._ui = UI(self._mock_builder,
        mock_file_manager_factory,
        mock_file_selection_factory)
    self._mock_file_manager = self._ui._file_manager
    self._mock_file_selection = self._ui._file_selection

  def testBuilderUsesTheAppropriateFile(self):
    self._mock_builder.assertDidAddFromFile("argonaut.ui")

  def testDataAttributesPulledFromBuilder(self):
    widgets = ["directory-window",
        "file-icons",
        "directory-close-menu-item",
        "file-open-menu-item",
        "file-uncompress-menu-item",
        "file-rename-menu-item",
        "file-copy-menu-item",
        "file-link-menu-item",
        "file-properties-menu-item",
        "file-delete-menu-item",
        "edit-cut-menu-item"]

    self.assertDataAccessor("directory_window", self._ui)
    for widget in widgets:
      self._mock_builder.assertDidGetObject(widget)

  def testSetsSignals(self):
    self._ui.setup_signals()

    self._ui.directory_window.assertSignalConnected("delete-event",
        Gtk.main_quit)
    self._ui._directory_close_menu_item.assertSignalConnected("activate",
        Gtk.main_quit)
    self._ui._file_icons.assertSignalConnected("selection-changed",
        self._ui._file_selection.change_sensitivity)
    self._ui._file_open_menu_item.assertSignalConnected("activate",
        self._mock_file_manager.open_files)

  def testDesensitizationOfFileMenu(self):
    self._mock_file_selection.assertSensitized()
    self._ui.desensitize_file_menu()

    self._mock_file_selection.assertDesensitized()


  def assertDataAccessor(self, accessor, obj):
    self.assertTrue(accessor in dir(obj),
        "no accessor for %s exists in %s" % (accessor, obj))

if __name__ == "__main__":
  unittest.main()
