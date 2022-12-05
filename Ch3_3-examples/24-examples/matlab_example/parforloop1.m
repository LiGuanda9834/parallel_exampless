function parforloop1(n)
tic
a = zeros(n);
parfor i = 1:n
    a(i) = max(abs(eig(rand(n))));
end
toc
