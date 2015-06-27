float fabs0(float a)
{
  if (a < 0.f)
    return a * -1.f;
  return a;
}

float fabs1(float a)
{
  if (a < 0)
    return a * -1;
  return a;
}

float fabs2(float a)
{
  int tmp = *(int*)&a;
  tmp &= 0x7FFFFFFF;
  return *(float*)&tmp;
}

float fabs3(float a)
{
  int tmp = *(int*)&a;
  tmp &= 0x7FFFFFFF;
  return *(float*)&tmp;
}


main()
{
  float f = -3.0;
  int x = *(int*)&f;
  x &= 0x7FFFFFFF;
  printf("%f\n", *(float*)&x);
}

