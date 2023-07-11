s.boot; // Execute (Cmd Enter) this line to boot the server (or use Cmd B to boot the server)

MIDIClient.init;
MIDIIn.connectAll;

(
~licht = NetAddr("192.168.1.11", 3032);
t = TIYCS.new("192.168.1.2");
// t = TIYCS.new("127.0.0.1");
// t.setScreenOrder(2, 1, 0);
t.fullscreen();
t.automate();
~j = JoniskMain.new("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config").gui();
// t.welcomeTxt();
)

~j.sequence = ~j.patterns[0].mirror;



~j.sequence = (0..24);

~j.placementGui
t.gui;
~j.triggerNextInSeq



// ~j.lagTime = 30
~j.jonisks[23].synth.set(\lagTime, 0.00001);
~j.jonisks[23].trigger;
~j.jonisks[19].ota("Hertenkamp", ~pw);

// setOtaServer
~pw

~j.jonisks[23].deepSleep(1); // 1 min