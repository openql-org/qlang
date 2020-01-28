func main() {
  qint x, y;
  var z, w;
  
  // classic lang
  z = 0;
  w = z;
  write w;
  writeln;

  // for quantum
  x = 0;   // Init
  y = x;   // telep
  write y; // measure
  writeln;
  qfunc;   // external func

}
