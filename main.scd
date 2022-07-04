s.boot;
(
// t = TIYCS.new("192.168.1.2");
t = TIYCS.new("127.0.0.1");
// ~j = JoniskMain.new.gui;
t.setScreenOrder(2, 1, 0);
t.fullscreen();
t.automate;
)


(
t = TIYCS.new("192.168.1.2");
// t = TIYCS.new("127.0.0.1").gui;
// t.mappingGui(0);
// t.gui
~j = JoniskMain.new.gui;
// ~j.sequence = (0..23);
t.automate
)

t.setScreenOrder(0,1,2); // Einde
t.clearRoutines

t.mappingGui(0);

~j.jonisks.do{|j| j.synth.set(\noiseMul, 0.0)};

~j.slidersDict
~j.triggerNextInSeq();
~j.jonisks[0].synth.set(\amp, 0)
~j.configLights
~j.gui
// Tank-sequence
MIDIIn.doNoteOnAction(1, 14, 25, 127);
MIDIIn.doNoteOnAction(1, 14, 26, 127);
MIDIIn.doNoteOnAction(1, 14, 27, 127);
MIDIIn.doNoteOnAction(1, 14, 28, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 30, 127);
MIDIIn.doNoteOnAction(1, 14, 31, 127);
MIDIIn.doNoteOnAction(1, 14, 96, 127); // Start fireball

MIDIIn.doNoteOnAction(1, 14, 97, 127); // Feest trigger

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

// Full screen
t.fullscreen();
t.gui

t.counter.valueAction = 3
t.valyueById(2, 255);