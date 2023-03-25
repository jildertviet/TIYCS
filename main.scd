s.boot; // Execute (Cmd Enter) this line to boot the server (or use Cmd B to boot the server)

(
t = TIYCS.new("127.0.0.1");
// t = TIYCS.new("127.0.0.1");
// t.setScreenOrder(2, 1, 0);
// t.fullscreen();
t.automate();
~j = JoniskMain.new("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config").gui();
// t.welcomeTxt();
)

