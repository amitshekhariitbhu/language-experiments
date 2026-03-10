# enumerable_lazy.rb
# Ruby's Enumerable module is incredibly powerful, and lazy enumerators
# allow you to work with infinite sequences efficiently.

puts "=== Enumerable Power & Lazy Enumerators ==="
puts

# --- Infinite ranges ---
puts "1. INFINITE RANGES (Ruby 2.6+):"
inf_range = (1..)
puts "   (1..) is an infinite range"
puts "   (1..).class => #{inf_range.class}"
puts "   First 5: #{inf_range.first(5).inspect}"
puts "   (1..).include?(1_000_000) => #{inf_range.include?(1_000_000)}"
puts

# --- Without lazy: this would hang/run out of memory ---
puts "2. LAZY ENUMERATORS — process infinite sequences:"
# Without .lazy, this would try to map ALL integers (infinite!)
first_10_squares = (1..).lazy.map { |n| n ** 2 }.first(10)
puts "   (1..).lazy.map { |n| n**2 }.first(10)"
puts "   => #{first_10_squares.inspect}"
puts

# --- Chaining lazy operations ---
puts "3. CHAINING — Multiple lazy operations compose efficiently:"
result = (1..).lazy
  .select { |n| n.odd? }           # Only odd numbers
  .map { |n| n ** 2 }              # Square them
  .reject { |n| n.to_s.include?('9') }  # Reject those containing '9'
  .first(8)
puts "   Odd squares without digit 9:"
puts "   => #{result.inspect}"
puts "   Only computed exactly as many as needed!"
puts

# --- Lazy vs eager performance ---
puts "4. LAZY vs EAGER — demonstrating short-circuit evaluation:"

$eager_count = 0
$lazy_count = 0

# Eager: processes ALL elements through each step
(1..100).select { |n| $eager_count += 1; n.even? }
        .map { |n| $eager_count += 1; n * 2 }
        .first(5)

# Lazy: only processes elements until we have enough
(1..100).lazy.select { |n| $lazy_count += 1; n.even? }
             .map { |n| $lazy_count += 1; n * 2 }
             .first(5)

puts "   Finding first 5 even numbers doubled from 1..100:"
puts "   Eager operations: #{$eager_count}"
puts "   Lazy operations:  #{$lazy_count}"
puts "   Lazy did ~#{($eager_count.to_f / $lazy_count).round(1)}x less work!"
puts

# --- Custom infinite enumerator ---
puts "5. CUSTOM INFINITE ENUMERATOR — Fibonacci sequence:"

fibs = Enumerator.new do |yielder|
  a, b = 0, 1
  loop do
    yielder.yield a
    a, b = b, a + b
  end
end

puts "   First 15 Fibonacci numbers:"
puts "   #{fibs.first(15).inspect}"
puts "   Fib #50: #{fibs.lazy.drop(49).first}"
puts

# --- Enumerator::Yielder for coroutine-like behavior ---
puts "6. ENUMERATOR as coroutine — external iteration:"

counter = Enumerator.new do |y|
  i = 0
  loop do
    y.yield i
    i += 1
  end
end

puts "   Manual iteration with .next:"
puts "   counter.next => #{counter.next}"
puts "   counter.next => #{counter.next}"
puts "   counter.next => #{counter.next}"
puts "   (State is preserved between calls!)"
puts

# --- zip, chunk, each_cons, each_slice ---
puts "7. POWERFUL ENUMERABLE methods most people don't know:"

# each_cons: sliding window
puts "   each_cons(3) — sliding window:"
puts "   #{(1..6).each_cons(3).to_a.inspect}"

# each_slice: fixed-size chunks
puts "   each_slice(2) — chunking:"
puts "   #{(1..7).each_slice(2).to_a.inspect}"

# chunk: group consecutive elements
puts "   chunk — group consecutive:"
data = [1, 1, 2, 2, 2, 3, 1, 1]
chunked = data.chunk { |n| n }.map { |val, arr| [val, arr.length] }
puts "   #{data.inspect}.chunk => #{chunked.inspect}"

# count occurrences (compatible with Ruby 2.6+)
puts "   count occurrences with each_with_object:"
words = %w[cat dog cat bird dog cat]
counts = words.each_with_object(Hash.new(0)) { |w, h| h[w] += 1 }
puts "   #{words.inspect} => #{counts.inspect}"

# flat_map
puts "   flat_map — map + flatten:"
puts "   #{[[1,2],[3,4],[5]].flat_map { |a| a.map { |n| n * 10 } }.inspect}"
puts

# --- Lazy with zip on infinite sequences ---
puts "8. ZIPPING INFINITE SEQUENCES:"
naturals = (1..)
evens = (2..).lazy.select(&:even?)
odds = (1..).lazy.select(&:odd?)

# Zip two infinite sequences lazily
pairs = naturals.lazy.zip(evens.lazy)
puts "   Naturals zipped with evens (first 5):"
puts "   #{pairs.first(5).inspect}"
puts

# --- Enumerator chaining (Ruby 2.6+) ---
puts "9. ENUMERATOR CHAINING with + operator:"
combined = (1..3).each + (10..12).each
puts "   (1..3).each + (10..12).each => #{combined.to_a.inspect}"
puts

# --- The power of reduce/inject ---
puts "10. REDUCE — more than just sums:"
# Building a hash from an array
hash = %w[name Alice age 30 city Tokyo].each_slice(2).reduce({}) do |h, (k, v)|
  h.merge(k => v)
end
puts "   Array to hash with reduce:"
puts "   #{hash.inspect}"
