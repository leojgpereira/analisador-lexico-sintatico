int f()
{
  
  int a;
  float b;
  double c;
  char d;

  
  int *a;
  float **b;
  double *c;
  char *d;

  
  *a = 5;
  b = *a + 6;
  c = *a * *b;
  d = 20 + **c;

  b = 3;
}
void main()
{
   a = 10;
   b = 3;
   while (a){
     c = (a+b)*2;
     a = a + 1 + (b + (c + 1) * d);
     b = b*b;
   }
   a = 10;
   while (b)
     a = 10;
   a = 10;
}
