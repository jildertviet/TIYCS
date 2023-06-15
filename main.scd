s.boot; // Execute (Cmd Enter) this line to boot the server (or use Cmd B to boot the server)

(
// t = TIYCS.new("127.0.0.1");
// t = TIYCS.new("127.0.0.1");
// t.setScreenOrder(2, 1, 0);
// t.fullscreen();
// t.automate();
~j = JoniskMain.new("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config").gui();
// t.welcomeTxt();
)
~j.lagTime = 30
~j.jonisks[23].synth.set(\lagTime, 0.00001);
~j.jonisks[23].trigger;
~j.jonisks[23].ota("Hertenkamp", ~pw);
// setOtaServer
~pw

~j.jonisks[23].deepSleep(1)

(1/30)*1000
(
var msg = (0xFF!6) ++ [0x09] ++ 1000.asInteger.asInt16.asBytes ++ ([0, 0, 0, 0]!25).reshape(25*4) ++ "end";
~j.serial.putAll(msg);
)

400.asInteger.asInt16().asBytes

floor(400 / 256)