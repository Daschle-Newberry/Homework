//https://www.codewars.com/kata/5a00e05cc374cb34d100000d

const reverseSeq = (n: number): number[] => {
  let res: number[] = [];
  for(let i = n; i > 0; i--) res.push(i);
  return res;
};