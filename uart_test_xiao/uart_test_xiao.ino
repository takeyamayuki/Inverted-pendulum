
void setup() {
  Serial1.begin(115200);
}

void loop() {
  int c=255;
  Serial1.write(c);
  Serial1.flush(); // clear send buffer
}
