let jonisks = [];
let translateX = 0;
let translateY = 0;
let connections = [];

function setup() {
  createCanvas(windowWidth, windowHeight);
  for (let i = 0; i < 15; i++) {
    jonisks.push(new Jonisk(random(width), random(height), i + 1));
  }
}

function draw() {
  background(0);
  translate(translateX, translateY);
  drawGrid();
  drawConnections();
  for (let jonisk of jonisks) {
    jonisk.show();
    if (jonisk.selected) {
      if (mouseIsPressed) {
        jonisk.drag(mouseX - translateX, mouseY - translateY);
      }
    }
  }
}

function mousePressed() {
  let found = false;
  for (let jonisk of jonisks) {
    if (jonisk.clicked(mouseX - translateX, mouseY - translateY)) {
      found = true;
      if (keyIsDown(SHIFT)) {
        let selectedJonisks = jonisks.filter(j => j.selected);
        if (selectedJonisks.length === 2) {
          connections.push([selectedJonisks[0].id, selectedJonisks[1].id]);
          selectedJonisks[0].deselect();
          selectedJonisks[1].deselect();
        }
      }
      break;
    }
  }
  if (!found) {
    for (let jonisk of jonisks) {
      jonisk.deselect();
    }
  }
}

function keyPressed() {
  let step = keyIsDown(SHIFT) ? 10 : 100;
  if (keyCode === LEFT_ARROW) {
    translateX += step;
  } else if (keyCode === RIGHT_ARROW) {
    translateX -= step;
  } else if (keyCode === UP_ARROW) {
    translateY += step;
  } else if (keyCode === DOWN_ARROW) {
    translateY -= step;
  } else if (keyCode === ENTER) {
    logPositions();
  } else if (key === 'n') {
    let input = prompt("Enter JSON string:");
    if (input !== null) {
      try {
        let data = JSON.parse(input);
        populateJonisks(data);
      } catch (e) {
        console.log("Invalid JSON string");
      }
    }
  }
}

function logPositions() {
  let positions = [];
  for (let jonisk of jonisks) {
    let connection = connections.find(c => c.includes(jonisk.id));
    let connectionId = null;
    if (connection) {
      connectionId = connection[0] === jonisk.id ? connection[1] : connection[0];
    }
    positions.push({
      xPos: jonisk.x,
      yPos: jonisk.y,
      id: jonisk.id,
      connectionId: connectionId
    });
  }
  console.log(JSON.stringify(positions));
}

function populateJonisks(data) {
  jonisks = [];
  connections = [];
  for (let item of data) {
    jonisks.push(new Jonisk(item.xPos, item.yPos, item.id));
    if (item.connectionId) {
      connections.push([item.id, item.connectionId]);
    }
  }
}

function drawGrid() {
  stroke(255,25.5);
  strokeWeight(1);

   for(let i=-2000; i<width+2000; i+=100){
    line(i,-2000,i,height+2000);

    for(let j=-2000; j<height+2000; j+=100){
      line(i,j,i+100,j);
      line(i,j,i,j+100);
    }

    line(i,height+1999,i+100,height+1999);

   }

   for(let j=-2000; j<height+2000; j+=100){
     line(-2000,j,width+2000,j);
     line(width+1999,j,width+1999,j+100);
   }
}

function drawConnections() {
  stroke(255);
  strokeWeight(2);

   for(let connection of connections){
     let j1 = jonisks.find(j => j.id === connection[0]);
     let j2 = jonisks.find(j => j.id === connection[1]);
     line(j1.x,j1.y,j2.x,j2.y);
   }
}

class Jonisk {
  constructor(x, y, id) {
    this.x = x;
    this.y = y;
    this.r = 20;
    this.selected = false;
    this.strokeWeight = 1;
    this.offsetX = 0;
    this.offsetY = 0;
    this.id = id;
  }

  show() {
    stroke(255);
    strokeWeight(this.strokeWeight);
    noFill();
    ellipse(this.x, this.y, this.r * 2);
    fill(255);
    noStroke();
    textAlign(CENTER, CENTER);
    text(this.id, this.x, this.y);
  }

  clicked(px, py) {
    let d = dist(px, py, this.x, this.y);
    if (d < this.r) {
      this.strokeWeight = 4;
      this.offsetX = this.x - px;
      this.offsetY = this.y - py;
      this.selected = true;
      return true;
    } else {
      this.selected = false;
      this.strokeWeight = 1;
      return false;
    }
  }

  drag(px, py) {
    this.x = px + this.offsetX;
    this.y = py + this.offsetY;
  }

  deselect() {
    this.selected = false;
    this.strokeWeight = 1;
  }
}
