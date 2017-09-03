Include = '../../../data/baxter_model/baxter.ors'
Delete shape visual

shape baxterR (right_wrist){ rel=<T d(-90 0 1 0) d(-90 0 0 1) t(0 0 -.26)> type=5 size=[.1 0 0 0] color=[1 1 0] }
shape baxterL (left_wrist) { rel=<T d(-90 0 1 0) d(-90 0 0 1) t(0 0 -.26)> type=5 size=[.1 0 0 0] color=[1 1 0] }

body target { X=<T t(.5 -.5 1.2) q(.939 0 0 .34) d(40 1 0 0)>  type=0 size=[.05 .1 .2 .05] color=[0 .5 0] fixed, }
#body target { X=<T t(.5 -1. 2.) d(-20 0 0 1)>  type=0 size=[.3 .1 .2 .05] color=[0 .5 0] fixed, }

