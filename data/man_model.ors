
## torso & arms

body waist { X=<T t(0 0 1.) > rel=<T d(90 0 1 0)> type=2 mass=1 size=[0. 0. .2 .15] }

body back     { type=2 mass=1 size=[0. 0. .15 .15] }
body chest    { type=2 mass=1 size=[0. 0. .3 .15] }
body shoulders{ type=2 mass=1 size=[0. 0. .3 .15] }
body shoulderL{ type=1 mass=.1 size=[0. 0. .15 .12]  }
body shoulderR{ type=1 mass=.1 size=[0. 0. .15 .12]  }
body upArmL   { type=2 mass=.1 size=[.15 .15 .15 .075]  }
body upArmR   { type=2 mass=.1 size=[.15 .15 .15 .075]  }
body dnArmL   { type=2 mass=.1 size=[.15 .15 .15 .075]  }
body dnArmR   { type=2 mass=.1 size=[.15 .15 .15 .075]  }
body upWristL { type=2 mass=.1 size=[.15 .15 .15 .06] contact }
body upWristR { type=2 mass=.1 size=[.15 .15 .15 .06] contact }

body neck { type=2 mass=.1 size=[.0 .0 .1 .08] }
body manhead { type=1 mass=.1 size=[0 0 0 .2] }

joint (waist back) { A=<T t(0 0 .075)> B=<T t(0 0 .075)>  }
joint (back chest) { A=<T t(0 0 .075) d(90 0 0 1)> B=<T d(-90 0 0 1) t(0 0 .15)>  }
joint (chest shoulders) { A=<T t(0 0 .15) d(90 0 1 0)> B=<T t(-.075 0 0)>  }
joint (shoulders shoulderL) { A=<T t(-.03 0 .22) d(-90 0 0 1) d(30 1 0 0)> B=<T d(90 0 0 1)>  }
joint (shoulders shoulderR) { A=<T d(180 0 1 0) t(.03 0 .22) d(90 0 0 1) d(30 1 0 0)> B=<T d(-90 0 0 1)>  }

joint (shoulderL upArmL){ A=<T d(90 0 1 0) t(-.02 0 .075)> B=<T t(0 0 .075)>  }
joint (shoulderR upArmR){ A=<T d(-90 0 1 0) t(.02 0 .075)> B=<T t(0 0 .075)>  }
joint (upArmL dnArmL)   { A=<T t(0 0 .075) d(-90 0 1 0) d(30 1 0 0)> B=<T d(90 0 1 0) t(0 0 .075)>  }
joint (upArmR dnArmR)   { A=<T t(0 0 .075) d( 90 0 1 0) d(30 1 0 0)> B=<T d(-90 0 1 0) t(0 0 .075)>  }
joint (dnArmL upWristL) { A=<T t(0 0 .075) d(80 1 0 0)> B=<T t(0 0 .075)>  }
joint (dnArmR upWristR) { A=<T t(0 0 .075) d(80 1 0 0)> B=<T t(0 0 .075)>  }

joint (shoulders neck) { A=<T t(-.075 0 0)> B=<T d(-90 0 1 0) t(0 0 .075)>  }
joint (neck manhead) { A=<T t(0 0 .075)> B=<T t(0 0 .15)>  }


## left & right hand

body dnWristR { type=2 mass=.01 size=[.1 .1 .1 .055] }
body dnWristL { type=2 mass=.01 size=[.1 .1 .1 .055] }
body ddnWristR{ type=2 mass=.01 size=[.5 .5 .04 .05] }
body ddnWristL{ type=2 mass=.01 size=[.5 .5 .04 .05] }
body handR    { type=9 mass=.01 size=[.1 .04 .1 .02] }
body handL    { type=9 mass=.01 size=[.1 .04 .1 .02] }

joint (upWristR dnWristR) { A=<T t(0 0 .075) d( 90 0 1 0) d(140 1 0 0)> B=<T d(-90 0 1 0) t(0 0 .075)>  }
joint (upWristL dnWristL) { A=<T t(0 0 .075) d(-90 0 1 0) d(140 1 0 0)> B=<T d( 90 0 1 0) t(0 0 .075)>  }
joint (dnWristR ddnWristR){ A=<T t(0 0 .075) d( 90 0 0 1)> B=<T d(-90 0 0 1) t(0 0 .03)>  }
joint (dnWristL ddnWristL){ A=<T t(0 0 .075) d(-90 0 0 1)> B=<T d( 90 0 0 1) t(0 0 .03)>  }
joint (ddnWristR handR)   { A=<T t(0 0 .045)> B=<T t(0 0 .075) d(180 0 1 0)>  }
joint (ddnWristL handL)   { A=<T t(0 0 .045)> B=<T t(0 0 .075) d(180 0 1 0)>  }

## legs

body lhip { mass=1 size=[.15 .15 .03 .12] type=2 }
body rhip { mass=1 size=[.15 .15 .03 .12] type=2 }
body lup  { mass=1 size=[.15 .15 .26 .11] type=2 } 
body rup  { mass=1 size=[.15 .15 .26 .11] type=2 } 
body ldn  { mass=1 size=[.15 .15 .45 .09] type=2 } 
body rdn  { mass=1 size=[.15 .15 .45 .09] type=2 } 
body lfoot{ mass=1 size=[.15 .45 .05 .02] type=9 }
body rfoot{ mass=1 size=[.15 .45 .05 .02] type=9 fixed }

joint (waist lhip) { A=<T t(-.15 0 -.08) d(90 0 0 1)> B=<T d(90 0 0 1) t(0 0 -.015)>  }
joint (waist rhip) { A=<T t(+.15 0 -.08) d(90 0 0 1)> B=<T d(90 0 0 1) t(0 0 -.015)>  }
joint (lhip lup) { A=<T t(0 0 -.015) d(20  1 0 0)> B=<T t(0 0 -.19)>  }
joint (rhip rup) { A=<T t(0 0 -.015) d(20  1 0 0)> B=<T t(0 0 -.19)>  }
joint (lup ldn)  { A=<T t(0 0 -.19) d(-40 1 0 0)> B=<T t(0 .015 -.23)>  }
joint (rup rdn)  { A=<T t(0 0 -.19) d(-40 1 0 0)> B=<T t(0 .015 -.23)>  }
joint (ldn lfoot){ A=<T t(0 0 -.25) d(20  1 0 0)> B=<T t(0 .06 -.038)>  }
joint (rdn rfoot){ A=<T t(0 0 -.25) d(20  1 0 0)> B=<T t(0 .06 -.038)>  }
