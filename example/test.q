func main() {
  qint x, y;
  var z;

  x = 0;
  y = 1;
  x = y;

  if x == 1 {
    write x;
  }

  while x <= 10 {
    write x;
    x = x + 1;
  }
  
  x = 1;

  if x <> 1 {
    write x;
  }
}