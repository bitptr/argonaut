import unittest

from argonautics.menu_sensitizer import MenuSensitizer
from argonautics.test.support.mock_menu_item import *
from argonautics.test.support.mock_icon_view import *

class TestMenuSensitizer(unittest.TestCase):
  def testSetsSensitivityForEachMenuItem(self):
    mock_menu_item1 = MockMenuItem(self)
    mock_menu_item2 = MockMenuItem(self)
    file_selection = MenuSensitizer([mock_menu_item1, mock_menu_item2])

    file_selection.set_sensitivity_to(True)
    mock_menu_item1.assertSensitized()
    mock_menu_item2.assertSensitized()

    file_selection.set_sensitivity_to(False)
    mock_menu_item1.assertDesensitized()
    mock_menu_item2.assertDesensitized()

  def testKnowsHowToSetSensitivityForAnIconView(self):
    mock_menu_item = MockMenuItem(self)
    file_selection = MenuSensitizer([mock_menu_item])
    mock_selected_icon_view = MockIconView(self, get_selected_items=[1])
    mock_unselected_icon_view = MockIconView(self, get_selected_items=[])

    file_selection.change_sensitivity(mock_selected_icon_view)
    mock_menu_item.assertSensitized()

    file_selection.change_sensitivity(mock_unselected_icon_view)
    mock_menu_item.assertDesensitized()


if __name__ == "__main__":
  unittest.main()
