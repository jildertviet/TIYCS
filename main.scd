s.boot; // Execute (Cmd Enter) this line to boot the server (or use Cmd B to boot the server)

(
t = TIYCS.new("192.168.1.2");
// t = TIYCS.new("127.0.0.1");
// t.setScreenOrder(2, 1, 0);
t.fullscreen();
t.automate();
~j = JoniskMain.new.gui();
t.welcomeTxt();
)