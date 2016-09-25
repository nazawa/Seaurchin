#pragma once

class Easing
{
public:

    static double Linear(double time, double timeDuration, double startValue, double valueDuration);
    static double InQuad(double time, double timeDuration, double startValue, double valueDuration);
    static double OutQuad(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutQuad(double time, double timeDuration, double startValue, double valueDuration);
    static double InCubic(double time, double timeDuration, double startValue, double valueDuration);
    static double OutCubic(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutCubic(double time, double timeDuration, double startValue, double valueDuration);
    static double InQuart(double time, double timeDuration, double startValue, double valueDuration);
    static double OutQuart(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutQuart(double time, double timeDuration, double startValue, double valueDuration);
    static double InQuint(double time, double timeDuration, double startValue, double valueDuration);
    static double OutQuint(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutQuint(double time, double timeDuration, double startValue, double valueDuration);
    static double InSine(double time, double timeDuration, double startValue, double valueDuration);
    static double OutSine(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutSine(double time, double timeDuration, double startValue, double valueDuration);
    static double InExpo(double time, double timeDuration, double startValue, double valueDuration);
    static double OutExpo(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutExpo(double time, double timeDuration, double startValue, double valueDuration);
    static double InCircle(double time, double timeDuration, double startValue, double valueDuration);
    static double OutCircle(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutCircle(double time, double timeDuration, double startValue, double valueDuration);
    static double InElastic(double time, double timeDuration, double startValue, double valueDuration);
    static double OutElastic(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutElastic(double time, double timeDuration, double startValue, double valueDuration);
    static double InBack(double time, double timeDuration, double startValue, double valueDuration);
    static double OutBack(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutBack(double time, double timeDuration, double startValue, double valueDuration);
    static double InBounce(double time, double timeDuration, double startValue, double valueDuration);
    static double OutBounce(double time, double timeDuration, double startValue, double valueDuration);
    static double InOutBounce(double time, double timeDuration, double startValue, double valueDuration);

    typedef double(*EasingFunction)(double, double, double, double);
};