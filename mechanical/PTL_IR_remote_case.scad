/* =====================================================================================
 *
 *       Filename:  sPTL_ir_remote_case.scad
 *
 *    Description:  PTL IR remote control case for LemanMake 2018 PTL workshop
 *
 *        Version:  1.0
 *        Created:  02 09 2018
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sebastien Chassot (sinux), sinux@posttenebraslab.ch.ch
 *        Company:  Post Tenebras Lab (Geneva's Hackerspace)
 *
 * =====================================================================================
 */


case();
//esp();
//esp_clearance();

module case (){
color([0,1,0])
difference(){
    case_full();
    esp_clearance();
}
}

/**
 *    Case without clearance
 **/
module case_full () {

    difference(){
        translate([0,-16,5])
        rotate([-90,0,0])
        intersection(){
    
            rotate_extrude(convexity = 10, $fn=80)import(file = "ovoid.dxf");
    
            translate([0,0,35])
                scale([1,1,2])sphere(d=40, $fn=80);
        }
        translate([-50,-50,-27])
            cube([100,100,20]);
        
        //truncate
        translate([-20,-15,-20])
            cube([40,10,40]);
        
        translate([0, 26, 47])
        rotate([0, 90, 0])
            cylinder(d=60, h=100, center=true, $fn=100);
    }
}


/**
 *    wemos + shield
 **/
module esp () {
    
    union() {
        // wemos
        color([0,0,1])
        translate([-25.4/2,-7,-2])
            cube([25.4,35,2]);
        
        // headers
        color([0,0,0])
        translate([-25.4/2,0,0])
            cube([2.54,23,11.5]);
        color([0,0,0])
        translate([25.4/2-2.54,0,0])
            cube([2.54,23,11.5]);
        
        // shield
        color([0,0,1])
        translate([0,0,11.5])
            linear_extrude(2)import(file = "shield.dxf");
        
        // DHT
        translate([-9.5,15,7.5]){
            color([1,1,1])
            translate([-1,0,6])
                cube([19,20,7.5]);
            
            color([1,1,1])
            translate([-1,.5,6])
                linear_extrude(2)import(file = "dht.dxf");
        }
    }
}

/**
 *    wemos clearance
 **/
module esp_clearance () {
    color([1,0,0])
    union() {
         // shield
        translate([0, 0, -2])
            linear_extrude(15.5)import(file = "shield.dxf");
        
        translate([-25.4/2, -30, -2])
            cube([25.4, 60, 2]);
        
        // shield space
        translate([0, -3, 13.5])
            linear_extrude(2.5)import(file = "shield_space.dxf");
            
        // DHT
        translate([-10,14.5,13.5]){
            color([1,1,1])
            translate([-1,0,0])
                cube([20,22,14]);
            
            color([1,1,1])
            translate([-1,.5,0])
                linear_extrude(2)import(file = "dht.dxf");
        };
    
    
        // clear space
        translate([-9.5,-17,-2]) 
            cube([20,28,27]);
        
        // clip
        translate([-11,-15,-4]) 
            cube([22,70,4]);
        translate([-25.4/2,0,-7])
            cube([25.4,45,5]);
    
        // USB wire
        translate([0,47,3])
        rotate([90,0,0]) {
            cylinder(d=10, h=10, $fn=100);
            translate([0, -4, 5])
                cube([10, 8, 10],center=true);
        }

        // photoresistor
        translate([7,9.5,0])
            cylinder(d=8, h=40);
        
        
        // reset button
        translate([20,24,-3])
            sphere(d=18);
    }
}


