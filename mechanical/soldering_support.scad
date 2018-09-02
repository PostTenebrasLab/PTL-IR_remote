/* =====================================================================================
 *
 *       Filename:  soldering_support.scad
 *
 *    Description:  Soldering support for LemanMake 2018 PTL workshop
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

LEDS_ANGLE=3;

support();

module support() {
    union() {
        difference() {    
            translate([0, 0, 35/2-7])
                rotate([90, 0, 0])
                    cylinder(d=35, h=20, $fn=100);

            union() {
                translate([6.7, 1, 11.5])
                    rotate([90+LEDS_ANGLE, 0, 0]) {
                        cylinder(d1=5.7, d2=4.7, h=12, $fn=100);
                        cylinder(d=3, h=22, $fn=100);
                    };

                translate([6.7, 1, 19.5])
                    rotate([90-LEDS_ANGLE, 0, 0]) {
                        cylinder(d1=5.7, d2=4.7, h=12, $fn=100);
                        cylinder(d=3, h=22, $fn=100);
                    };

                translate([0, 0, -15])
                    cube([50, 50, 30], center=true);
                cube([26.5, 15, 4], center=true);
                
                rotate([0, 90, 0]) {
                    translate([0, 0, -27.7]) {
                        cube([50, 50, 30], center=true);
                    }
                    
                }
               // cube([26.5, 15, 4], center=true);
            }
        }

        translate([0, 0, 16])
            rotate([0, 90, 0]) {
                translate([-3, -1, -1])
                    cube([2, 5, 4]);
                translate([1, -1, -1])
                    cube([2, 5, 4]);
            }
    }
}