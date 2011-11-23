from gi.repository import Gio

class DirectoryMonitor:
  def __init__(self, directory, directory_filler):
    self._directory = directory
    self._directory_filler = directory_filler
    self._cached_monitor = None

  def start_watching(self):
    self._monitor().connect("changed", self._directory_changed)

  def _directory_changed(self, _1, _2, _3, _4):
    self._directory_filler.refill_store()

  def _monitor(self):
    if self._cached_monitor is None:
      self._cached_monitor = self._directory.monitor(Gio.FileMonitorFlags.NONE, None)
    return self._cached_monitor
