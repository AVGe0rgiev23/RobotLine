// Computer tests for firmware/ZumoLineFollower/LineTracking.h.
// Run:  g++ -std=c++11 -Wall -o /tmp/t tests/test_line_tracking.cpp && /tmp/t
// (scripts/check.sh runs it automatically if g++ is installed.)
//
// Sensor values: 0 = white .. 1000 = black, sensors left (0) to right (5).

#include <cstdio>
#include "../firmware/ZumoLineFollower/LineTracking.h"

static int failures = 0;

static void expectNear(const char *name, int got, int want, int tolerance)
{
  int diff = got - want;
  if (diff < 0) diff = -diff;
  if (diff > tolerance)
  {
    std::printf("FAIL %s: got %d, want %d (+/- %d)\n", name, got, want, tolerance);
    failures++;
  }
  else
  {
    std::printf("ok   %s: %d\n", name, got);
  }
}

static void expectBool(const char *name, bool got, bool want)
{
  if (got != want)
  {
    std::printf("FAIL %s: got %s, want %s\n", name, got ? "true" : "false", want ? "true" : "false");
    failures++;
  }
  else
  {
    std::printf("ok   %s: %s\n", name, got ? "true" : "false");
  }
}

int main()
{
  const unsigned int L = 400;  // patch level used by the robot

  // --- Following a single line --------------------------------------
  unsigned int centred[6]   = {  0,   0, 900, 900,   0,   0 };
  unsigned int leftish[6]   = {  0, 300, 950, 200,   0,   0 };
  unsigned int rightEdge[6] = {  0,   0,   0,   0, 300, 1000 };
  unsigned int between[6]   = {  0,   0, 300, 1000, 300,  0 };
  expectNear("centred line", trackedPosition(centred, 2500, L), 2500, 1);
  expectNear("line slightly left", trackedPosition(leftish, 2500, L), 1900, 150);
  expectNear("line at right edge", trackedPosition(rightEdge, 4000, L), 4770, 100);
  expectNear("line between sensors (smooth)", trackedPosition(between, 2500, L), 3000, 1);

  // --- Crossing line approaching diagonally (wobble cause) -----------
  // Our line is centred; the crossing line already shows at the right edge,
  // as a separate patch. The robot must keep steering by OUR line.
  unsigned int crossAhead[6] = {  0,   0, 900, 800,   0, 900 };
  expectNear("crossing ahead on the right is ignored", trackedPosition(crossAhead, 2500, L), 2500, 150);
  unsigned int crossAheadL[6] = { 900,   0, 800, 900,   0,   0 };
  expectNear("crossing ahead on the left is ignored", trackedPosition(crossAheadL, 2500, L), 2500, 150);
  expectBool("crossing ahead on one side is NOT yet a crossing", spansBothSides(crossAhead, L), false);

  // --- Crossing / zebra stripe / finish under the bar -----------------
  unsigned int fullBlack[6] = { 900, 900, 900, 900, 900, 900 };
  unsigned int crossingTilted[6] = { 800, 0, 900, 900, 700, 900 };
  unsigned int crossingEdges[6] = { 800, 0, 0, 0, 0, 900 };
  unsigned int crossing0and4[6] = { 800, 0, 0, 0, 900, 0 };
  expectBool("full black bar = crossing", spansBothSides(fullBlack, L), true);
  expectBool("tilted crossing = crossing", spansBothSides(crossingTilted, L), true);
  expectBool("only both outer sensors dark = crossing", spansBothSides(crossingEdges, L), true);
  expectBool("sensors 0 and 4 dark = crossing", spansBothSides(crossing0and4, L), true);

  // --- Sharp corners must NOT look like crossings --------------------
  // 90-degree corner to the right: incoming line in the middle, the new
  // direction lies along the right half of the bar.
  unsigned int cornerRight[6] = {   0,   0, 900, 900, 900, 900 };
  unsigned int cornerLeft[6]  = { 900, 900, 900, 900,   0,   0 };
  unsigned int lineAtLeft[6]  = { 900, 700,   0,   0,   0,   0 };
  unsigned int sensors1and4[6] = {  0, 900, 0, 0, 900, 0 };
  expectBool("sharp corner right is NOT a crossing", spansBothSides(cornerRight, L), false);
  expectBool("sharp corner left is NOT a crossing", spansBothSides(cornerLeft, L), false);
  expectBool("line at the left edge is NOT a crossing", spansBothSides(lineAtLeft, L), false);
  expectBool("sensors 1 and 4 only is NOT a crossing", spansBothSides(sensors1and4, L), false);
  // In a corner the dark patch is connected to our line, so the robot steers into it.
  expectNear("corner right pulls steering right", trackedPosition(cornerRight, 2500, L), 3500, 400);
  expectNear("corner left pulls steering left", trackedPosition(cornerLeft, 2500, L), 1500, 400);

  // --- No line --------------------------------------------------------
  unsigned int white[6] = { 50, 80, 60, 40, 90, 70 };
  expectNear("all white = no line (-1)", trackedPosition(white, 2500, L), -1, 0);
  expectBool("all white is NOT a crossing", spansBothSides(white, L), false);

  if (failures == 0) std::printf("All line-tracking tests passed.\n");
  else std::printf("%d line-tracking test(s) FAILED.\n", failures);
  return failures == 0 ? 0 : 1;
}
