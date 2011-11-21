class MockWidget:
  def __init__(self, test_case, widget_name):
    self._test_case = test_case
    self._widget_name = widget_name
    self._widget_connections = {}

  def connect(self, signal_name, callback):
    self._widget_connections[signal_name] = callback

  def assertSignalConnected(self, signal_name, callback):
    self._test_case.assertTrue(self._widget_connections.has_key(signal_name),
        "the signal %s for the widget %s is not set" % (signal_name, self._widget_name))
    self._test_case.assertEqual(self._widget_connections[signal_name], callback,
        "the signal %s for the widget %s is set to %s instead of %s" % (signal_name, self._widget_name, self._widget_connections[signal_name], callback))
