#pragma once

void viewportInit(Viewport& vp, const Scene* Sc);
void viewportCalcFlags(const Viewport& vp, Vertex* v);
void viewportCalcYFlags(const Viewport& vp, Vertex* v);
