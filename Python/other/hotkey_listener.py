from pynput.keyboard import GlobalHotKeys


class HotKeyListener:
    def __init__(self):
        self.listener = []

    def add(self,  shortcut, function):
        self.listener.append(GlobalHotKeys({shortcut: function}))#, '<ctrl>+<alt>+a': function}))

        self.listener[-1].start()

    def stop(self):
        for l in self.listener:
            l.stop()