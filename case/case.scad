$fn=100;
width=60;
height=60;
depth=50;

wall=2;

// display
display_width=35;
display_height=20;
display_x=5;
display_y=-30;
display_z=-12;

// sensor
sensor_width=34;
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


difference() { // case
  cube([width,depth,height], center=true);
  translate ([0,wall/2,0]) cube([width-wall,depth,height-wall], center=true);  
  
  translate([display_x,display_y+wall,display_z]) cube([display_width, 10, display_height], center=true);

  translate([sensor_x,sensor_y+wall,sensor_z]) cube([sensor_width, 10, sensor_height], center=true);

  translate([usb_x,usb_y,usb_z]) rotate([90,0,0]) cube([usb_width, usb_height, 10], center=true);

  translate([led_x,led_y,led_z]) cylinder(d=led_diameter, h=wall*2, center=true);

}

// rail for back cover
difference() {
  translate([0,depth/2+wall,0]) cube([width, wall*2, height], center=true);
  translate([0,depth/2+wall*2,10]) cube([width-wall*5, wall*5, height+wall*5], center=true);
  translate([0,depth/2+wall/2,10]) cube([width-wall, wall*2, height+wall*9], center=true);}