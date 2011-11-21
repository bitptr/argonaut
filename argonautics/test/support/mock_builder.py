class MockBuilder:
  def __init__(self):
    self.added_from_file = None
    self.got_object = []

  def add_from_file(self,filename):
    self.added_from_file = filename

  def get_object(self, widget_name):
    self.got_object.append(widget_name)

  def did_add_from_file(self, filename):
    return self.added_from_file == filename

  def did_get_object(self, widget_name):
    return widget_name in self.got_object
