s.boot;

(
t = TIYCS.new("192.168.1.2").gui;
// t = TIYCS.new("127.0.0.1").gui;
// t.gui
~j = JoniskMain.new.gui;
// ~j.sequence = [	4,5,9,10]; // Den Dolder
~j.sequence = (0..23);
)
~j.slidersDict
~j.triggerNextInSeq();
~j.jonisks[0].synth.set(\amp, 0)
~j.configLights
// Tank-sequence
MIDIIn.doNoteOnAction(1, 14, 25, 127);
MIDIIn.doNoteOnAction(1, 14, 26, 127);
MIDIIn.doNoteOnAction(1, 14, 27, 127);
MIDIIn.doNoteOnAction(1, 14, 28, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 30, 127);
MIDIIn.doNoteOnAction(1, 14, 31, 127);

MIDIIn.doNoteOnAction(1, 14, 36, 127); // Jonisk seq (ARP)

MIDIIn.doNoteOnAction(1, 14, 35, 127); // Alarm on
MIDIIn.doNoteOffAction(1, 14, 35, 127); // Alarm off

MIDIIn.doNoteOnAction(1, 14, 34, 127); // Captain scene

MIDIIn.doNoteOnAction(1, 14, 19, 127); // Shopping (when Commercials scene is active)
MIDIIn.doNoteOnAction(1, 14, 20, 127); // Commercial 0

MIDIIn.doNoteOnAction(1, 1, 19, 127); // Shopping-txt (when Commercials scene is active)

// Set tex via OSC
(
var screen = 0;
t.setTexCoord(screen, id: 0, x: 10, y: 46);
t.setTexCoord(screen, id: 1, x: 1280-100, y: 0);
t.setTexCoord(screen, id: 2, x: 1280-40, y: 800-0);
t.setTexCoord(screen, id: 3, x: 4, y: 800-0);
)
(
var screen = 1;
t.setTexCoord(screen, id: 0, x: 100, y: 0);
t.setTexCoord(screen, id: 1, x: 1280-150, y: 10);
t.setTexCoord(screen, id: 2, x: 1280-100, y: 800-80);
t.setTexCoord(screen, id: 3, x: 60, y: 800-100);
)
(
var screen = 2;
t.setTexCoord(screen, id: 0, x: 90, y: 65);
t.setTexCoord(screen, id: 1, x: 1280-40, y: 10);
t.setTexCoord(screen, id: 2, x: 1280-10, y: 800-70);
t.setTexCoord(screen, id: 3, x: 80, y: 800-0);
)
// Full screen
t.allFullscreen();
t.gui

1280*3