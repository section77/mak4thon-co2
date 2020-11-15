$fn=100;

case=true;
back=true;

screwholes=false;  // optional; only needed if you want the case to be very close (or maybe unopenable without a screwdriver)

width=60;
height=60;
depth=40;

wall=2;

// display
display_width=34;
display_height=20;
display_x=5;
display_z=-12;

// sensor
sensor_width=33;
sensor_height=22;
sensor_x=0;
sensor_z=12;

// usb connector
usb_width=10;
usb_height=12;
usb_x=-33;
usb_z=-10;

// LED
led_diameter=5;
led_z=height/2-wall/2;

if (case) {
  difference() {
    union() {
      difference() { // case
        cube([width,depth,height], center=true);  // body
        translate ([0,wall/2,0]) cube([width-wall,depth,height-wall], center=true); // inner space
        translate([display_x,-depth/2,display_z]) cube([display_width, 10, display_height], center=true); // display
        translate([sensor_x,-depth/2,sensor_z]) cube([sensor_width, 10, sensor_height], center=true); // sensor
        translate([usb_x,-depth/2+wall*2,usb_z]) rotate([90,0,0]) cube([usb_width, usb_height, 10], center=true); // usb connector
        translate([0,0,led_z]) cylinder(d=led_diameter, h=wall*2, center=true); // led
      }
      translate([(width/2-wall*1.5), 0, (height/2-wall*1.5)]) corner();
      translate([-(width/2-wall*1.5), 0, -(height/2-wall*1.5)]) corner();
      translate([-(width/2-wall*1.5), 0, height/2-wall*1.5]) corner();
      translate([width/2-wall*1.5, 0, -(height/2-wall*1.5)]) corner();
    }
    //translate([0,depth*0.9,0]) cube([100,100,100], center=true); // block for print debugging
  }
}

module corner() { // corner blocks for screws
  width=4;
  hole=2.5;
  difference() {
    translate([0,0,0]) cube([width, depth, width], center=true);
    if (screwholes) translate([0,1,0]) rotate([90,0,0]) cylinder(d=hole, h=depth, center=true); // only if you want screwholes
  }
}

if (back) {
  difference() {
    hole=3.5;
    translate([0,depth,0]) union() {
      cube([width,wall,height], center=true); // back plate
      translate([0,-wall,height/2-wall]) cube([width-12,wall,wall], center=true); // top
      translate([0,-wall,-(height/2-wall)]) cube([width-12,wall,wall], center=true); // bottom
      translate([width/2-wall,-wall,0]) rotate ([0,90,0]) cube([width-12,wall,wall], center=true); // left
      translate([-(width/2-wall),-wall,0]) rotate ([0,90,0])  cube([width-12,wall,wall], center=true); // right   
    };
    if (screwholes) { // only if you want screwholes
      translate([width/2-wall*1.5,depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([-(width/2-wall*1.5),depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([-(width/2-wall*1.5),depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([width/2-wall*1.5,depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
    };
  };
}