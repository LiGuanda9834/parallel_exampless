function forloop1(n)
tic
a = zeros(n);
for i = 1:n
    a(i) = max(abs(eig(rand(n))));
end
toc
