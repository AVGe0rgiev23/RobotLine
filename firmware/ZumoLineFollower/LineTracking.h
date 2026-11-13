// LineTracking.h - small helpers that decide WHERE the line is and whether
// the robot is on a crossing. They only do arithmetic on the 6 calibrated
// sensor values (0 = white .. 1000 = black), so they can be tested on a
// computer too: see tests/test_line_tracking.cpp.
//
// Sensors, left to right: index 0 .. 5. Position: 0 (line under sensor 0)
// .. 5000 (line under sensor 5); 2500 = centred.

#ifndef LINE_TRACKING_H
#define LINE_TRACKING_H

const int LT_SENSORS = 6;

// Position of OUR line: the dark patch nearest to where the line was last
// time (lastPosition). Other dark patches - for example the edge of a crossing
// line that is still a few cm ahead - are ignored, so they cannot pull the
// steering sideways. A patch = neighbouring sensors above `level`; its
// position is the weighted average of the patch plus one sensor on each side
// (so the reading stays smooth as the line moves between two sensors).
// Returns -1 if no sensor is above `level`.
inline int trackedPosition(const unsigned int *v, int lastPosition, unsigned int level)
{
  int bestPosition = -1;
  long bestDistance = 0x7FFFFFFFL;
  int i = 0;
  while (i < LT_SENSORS)
  {
    if (v[i] <= level) { i++; continue; }
    int first = i;
    while (i < LT_SENSORS && v[i] > level) i++;
    int last = i - 1;                       // patch = sensors first..last
    int from = first > 0 ? first - 1 : first;
    int to = last < LT_SENSORS - 1 ? last + 1 : last;
    long sum = 0, weighted = 0;
    for (int k = from; k <= to; k++)
    {
      sum += v[k];
      weighted += (long)v[k] * k * 1000;
    }
    int position = (int)(weighted / sum);
    long distance = position - lastPosition;
    if (distance < 0) distance = -distance;
    if (distance < bestDistance)
    {
      bestDistance = distance;
      bestPosition = position;
    }
  }
  return bestPosition;
}

// True if dark reaches BOTH outer sides of the sensor bar at once: an outer
// sensor on one side and one of the two outer sensors on the other side
// (at least about 4.4 cm apart). A single 2 cm line can never do that, and the
// rules keep other lines at least 20 cm away, so it means: crossing, zebra
// stripe, start line or finish area.
inline bool spansBothSides(const unsigned int *v, unsigned int level)
{
  bool s0 = v[0] > level, s1 = v[1] > level;
  bool s4 = v[4] > level, s5 = v[5] > level;
  return (s0 && (s4 || s5)) || (s5 && (s0 || s1));
}

#endif
