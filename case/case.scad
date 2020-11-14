$fn=100;

case=false;
back=true;


width=60;
height=60;
depth=50;

wall=2;

// display
display_width=34;
display_height=20;
display_x=5;
display_y=-30;
display_z=-12;

// sensor
sensor_width=33;
sensor_height=22;
sensor_x=0;
sensor_y=-30;
sensor_z=12;

// usb connector
usb_width=10;
usb_height=12;
usb_x=-33;
usb_y=-21;
usb_z=-10;

// LED
led_diameter=6;
led_x=0;
led_y=0;
led_z=height/2-wall/2;

if (case) {
  difference() {
    union() {
      difference() { // case
        cube([width,depth,height], center=true);  // body
        translate ([0,wall/2,0]) cube([width-wall,depth,height-wall], center=true); // inner space
        translate([display_x,display_y+wall,display_z]) cube([display_width, 10, display_height], center=true); // display
        translate([sensor_x,sensor_y+wall,sensor_z]) cube([sensor_width, 10, sensor_height], center=true); // sensor
        translate([usb_x,usb_y,usb_z]) rotate([90,0,0]) cube([usb_width, usb_height, 10], center=true); // usb connector
        translate([led_x,led_y,led_z]) cylinder(d=led_diameter, h=wall*2, center=true); // led
      }
      translate([(width/2-wall*1.5), 0, (height/2-wall*1.5)], center=true) corner();
      translate([-(width/2-wall*1.5), 0, -(height/2-wall*1.5)], center=true) corner();
      translate([-(width/2-wall*1.5), 0, height/2-wall*1.5], center=true) corner();
      translate([width/2-wall*1.5, 0, -(height/2-wall*1.5)], center=true) corner();
    }
    //translate([0,30,0]) cube([100,100,100], center=true); // block for print debugging
  }
}

module corner() { // corner blocks for screws
  width=4;
  hole=2;
  difference() {
    translate([0,0,0]) cube([width, depth, width], center=true);
    translate([0,1,0]) rotate([90,0,0]) cylinder(d=hole, h=depth, center=true);
  }
}

if (back) {
  difference() {
    hole=3;
    translate([0,depth,0]) union() {
      cube([width,wall,height], center=true); // back plate
      translate([0,-wall,height/2-wall]) cube([width-12,wall,wall], center=true); // top
      translate([0,-wall,-(height/2-wall)]) cube([width-12,wall,wall], center=true); // bottom
      translate([width/2-wall,-wall,0]) rotate ([0,90,0]) cube([width-12,wall,wall], center=true); // left
      translate([-(width/2-wall),-wall,0]) rotate ([0,90,0])  cube([width-12,wall,wall], center=true); // right   
    };
    translate([width/2-wall*1.5,depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
    translate([-(width/2-wall*1.5),depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
    translate([-(width/2-wall*1.5),depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
    translate([width/2-wall*1.5,depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
  };
}