const reverseSeq = (n: number): number[] => {
  let res: number[] = [];
  for(let i = n; i > 0; i--) res.push(i);
  return res;
};