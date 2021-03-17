$fn=100;

case=true;
back=true;

screwholes=false;  // optional; only needed if you want the case to be very close (or maybe unopenable without a screwdriver)

width=60;
height=60;
depth=30;

wall=1;

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
usb_depth=8;
usb_x=-33;
usb_y=-depth/2+3;
usb_z=-11;

// LED
led_diameter=5;
led_z=height/2;

if (case) {
  
    difference() {
      union() {
        difference() { // case
          minkowski() {
            cube([width-wall,depth-1,height-wall], center=true);  // body
            rotate([90,0,0]) cylinder(r=2,h=1, center=true);
          } // rounded edge
          
          translate ([0,wall,0]) cube([width-wall,depth,height-wall], center=true); // inner space
          translate([display_x,-depth/2,display_z]) cube([display_width, 10, display_height], center=true); // display
          translate([sensor_x,-depth/2,sensor_z]) cube([sensor_width, 10, sensor_height], center=true); // sensor
          translate([usb_x,usb_y+wall*2,usb_z]) rotate([90,0,0]) cube([usb_width, usb_height, usb_depth], center=true); // usb connector
          translate([0,0,led_z]) cylinder(d=led_diameter, h=wall*4, center=true); // led
        }
        translate([(width/2-wall*1.5), 0, (height/2-wall*1.5)]) corner();
        translate([-(width/2-wall*1.5), 0, -(height/2-wall*1.5)]) rotate([0,180,0]) corner();
        translate([-(width/2-wall*1.5), 0, height/2-wall*1.5]) rotate([0,-90,0]) corner();
        translate([width/2-wall*1.5, 0, -(height/2-wall*1.5)]) rotate([0,90,0]) corner();
      }
      /*#translate([30,0,0]) cube([100,100,100], center=true); // block for print debugging
      #translate([0,0,30]) cube([100,100,100], center=true); // block for print debugging
      #translate([0,-50,0]) cube([100,100,100], center=true); // block for print debugging*/
    }
    
}

module corner() { // corner blocks for screws
  width=5;
  hole=2.5;
  difference() {
    translate([-1,0,-1]) cube([width, depth, width], center=true);
    if (screwholes) {
      translate([0,1,0]) rotate([90,0,0]) cylinder(d=hole, h=depth, center=true); // only if you want screwholes
    } else {
      translate([-3.9,1,-3.9]) rotate([90,0,0]) cylinder(d=10, h=depth, center=true);
    }
  }
}

if (back) {
  difference() {
    hole=3.5;
    translate([0,depth,0]) union() {
      minkowski() {
        cube([width-wall,wall/2,height-wall], center=true); // back plate
        rotate([90,0,0]) cylinder(r=2,h=1, center=true);
      } // rounded edge
      translate([0,-wall*2,height/2+wall/2-wall*2]) cube([width-12,wall*3,wall*2], center=true); // top
      translate([0,-wall*2,-(height/2-wall*1.5)]) cube([width-12,wall*3,wall*2], center=true); // bottom
      translate([width/2-wall*1.5,-wall*2,0]) rotate ([0,90,0]) cube([width-12,wall*3,wall*2], center=true); // left
      translate([-(width/2-wall*1.5),-wall*2,0]) rotate ([0,90,0])  cube([width-12,wall*3,wall*2], center=true); // right   
    };
    if (screwholes) { // only if you want screwholes
      translate([width/2-wall*1.5,depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([-(width/2-wall*1.5),depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([-(width/2-wall*1.5),depth,height/2-wall*1.5]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      translate([width/2-wall*1.5,depth,-(height/2-wall*1.5)]) rotate ([90,0,0]) cylinder(d=hole,h=10, center=true);
      /*#translate([30,0,0]) cube([100,100,100], center=true); // block for print debugging
      #translate([0,0,30]) cube([100,100,100], center=true); // block for print debugging
      #translate([0,-50,0]) cube([100,100,100], center=true); // block for print debugging*/
    };
  };
}
