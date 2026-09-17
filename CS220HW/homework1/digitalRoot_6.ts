//https://www.codewars.com/kata/541c8630095125aba6000c00

export const digitalRoot = (n:number):number => {
  if(n % 10 == n) return n;

  let places : number = (n == 0) ? 1 : Math.floor(Math.log10(n)) + 1;
  let sum: number = 0;
  let last: number = 0;

  for(let i: number = 10; i <= 10 ** places; i *= 10){
    let digit: number = ((n % i) - last)/(i/10);
    sum += digit;
    last = n % i;
  }

  return digitalRoot(sum);
}
