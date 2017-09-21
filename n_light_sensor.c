#include "n_light_sensor.h"

#define XN  95.045
#define YN  100.00
#define ZN  108.892

//static lch_raw_t Lch;

static int LCH_R_HIGH = 0;
static int LCH_G_HIGH = 0;
static int LCH_B_HIGH = 0;

static int L = 0;
static int C = 0;
static int H = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  PRIVATE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void change_rgb(rgb_raw_t* rgb, lch_raw_t* n_rgb)
{

    if((n_rgb->l = rgb->r / (double)LCH_R_HIGH) > 1.0) n_rgb->l = 1.0;
    if((n_rgb->c = rgb->g / (double)LCH_G_HIGH) > 1.0) n_rgb->c = 1.0;
    if((n_rgb->h = rgb->b / (double)LCH_B_HIGH) > 1.0) n_rgb->h = 1.0;

}

//f(t)を求める関数
static double f_collect(double t)
{
          if(t > pow((6.0 / 29.0), 3))
                    return (pow(t, (1.0 / 3.0)));

          return (1.0 / (3.0 * (pow((29.0 / 6.0), (2.0 * t)))) + (4.0 / 29.0));
}

static void lch_change_rgb_data(rgb_raw_t *rgb, lch_raw_t *lch)
{
    lch_raw_t n_rgb, xyz;

    change_rgb(rgb, &n_rgb);

    //あまりにも実値が小さすぎるため100倍をして整数の値に近くさせる。誤差は計算上切り捨てられてしまう
    xyz.l = (0.4124 * n_rgb.l + 0.3576 * n_rgb.c + 0.1805 * n_rgb.h) * 100;
    xyz.c = (0.2126 * n_rgb.l + 0.7152 * n_rgb.c + 0.0722 * n_rgb.h) * 100;
    xyz.h = (0.0193 * n_rgb.l + 0.1192 * n_rgb.c + 0.9505 * n_rgb.h) * 100;

    lch->l = 116.0 * f_collect(xyz.l / XN) - 16.0;
    lch->c = 500.0 * (f_collect(xyz.l / XN) - f_collect(xyz.c / YN));
    lch->h = 200.0 * (f_collect(xyz.c / YN) - f_collect(xyz.h / ZN));

    L = lch->l;
    C = lch->c;
    H = lch->h;
}



int judge_color(void){
    lch_raw_t g;
    
    //g.l明るさ(明度)、g.c彩度、g.h色相角
    g.l = L;
    g.c = C;
    g.h = H;
        // printf("L:%f,C:%f,h:%f\n",g.l,g.c,g.h);
      //   printf("V:%d\n",ev3_battery_voltage_mV());
      //   tslp_tsk(900);

    if(g.l>=93){//白の場合
        return 0;
    }else if(g.c>=45){//赤の場合
       // printf("RED\n"); 
        return 5;
          
    }else if(g.l>=65 && g.h>=55){//黄色の場合
       // printf("YELLOW\n");  
        return 4;
          
    }else if(g.l>=55 && g.h<=-25){//青色の場合
        // printf("BLUE\n");  
        return 2;
          
    }else if(g.l>=40 && g.l<=80 && g.h>=0 && g.h<=30 && g.c<=-20){//緑の場合
       // printf("GREEN\n");  
        return 3;
          
    }else{
       // printf("NaN\n");  
        return 0;
    }
}




//===========================================================================
//  PUBLIC
//===========================================================================

void n_lch_init(rgb_raw_t* rgb)
{
    LCH_R_HIGH = rgb->r;
    LCH_G_HIGH = rgb->g;
    LCH_B_HIGH = rgb->b;

}




void lch_get( lch_raw_t* lch ){
    rgb_raw_t rgb;
    
    ev3_color_sensor_get_rgb_raw(EV3_PORT_2, &rgb);
    lch_change_rgb_data(&rgb, lch);

}


//色情報を返す
int lch_color(){
    return judge_color();
}



void lch_high_color(rgb_raw_t* rgb)
{

    rgb->r = LCH_R_HIGH;
    rgb->g = LCH_G_HIGH;
    rgb->b = LCH_B_HIGH;

}

