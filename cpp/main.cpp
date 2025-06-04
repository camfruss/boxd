
// how to prune more ....
// remove any boxes that are = or larger in every dimension and cheaper
// replace uint8/16 with just int
// change to items = rows, boxes = columns
/*
- take any(item & subset)    TAKE | of all columns
     - if 0: terminate
- take MSB index, price++
- sum all prices
- keep priority queue of min 1000 prices
- std::array<uint8_fast_t, N> combination; float cost;

feasibility_matrix
return std::vector<float> prices
*/
#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

using namespace std::string_view_literals;


struct Item
{
    std::string id;
    std::array<int, 3> dims;
    float price {};

    bool operator<(const Item& rhs) const
    {
        return this->dims[0] < rhs.dims[0]
            && this->dims[1] < rhs.dims[1]
            && this->dims[2] < rhs.dims[2];
    }
};

/*
 * rows = products, columns = boxes
 */
std::vector<int> construct_feasibility_matrix(const std::vector<Item>& t_boxes, const std::vector<Item> t_products)
{
    std::vector<int> fm;  
    fm.reserve(t_boxes.size() * t_products.size());

    int i {};
    for (const auto& product : t_products)
    {
        for (const auto& box : t_boxes) 
        {
            fm[i++] = static_cast<int>(product < box);
        }
    }

    return fm;
}

Item parse_line(const std::string& t_line, bool t_hasPrice = false)
{
    Item item {};
    std::istringstream iss { t_line };
    std::string tmp {};  
    
    std::getline(iss, item.id, ',');

    std::getline(iss, tmp, ',');  item.dims[0] = static_cast<int>(std::stof(tmp)); // dim_1
    std::getline(iss, tmp, ',');  item.dims[1] = static_cast<int>(std::stof(tmp)); // dim_2
    std::getline(iss, tmp, ',');  item.dims[2] = static_cast<int>(std::stof(tmp)); // dim_3

    if (t_hasPrice)
    {
        std::getline(iss, tmp, ',');
        item.price = std::stof(tmp);
    }

    return item;
}

/* Requires columns be id, dim1, dim2, dim3, price?, ... */
std::pair<std::vector<Item>, std::vector<Item>> parse_csv(const std::string& t_boxPath, const std::string& t_productPath)
{
    std::vector<Item> boxes; boxes.reserve(1'000);
    std::vector<Item> products; products.reserve(1'000);

    std::ifstream ifs { t_boxPath };
    std::string line {};

    std::getline(ifs, line);  // skips header line 
    while (ifs.good() && std::getline(ifs, line))
    {
        Item box { parse_line(line, true) };
        boxes.push_back(box);
    }

    ifs.close(); ifs.clear();
    ifs = std::ifstream(t_productPath);

    std::getline(ifs, line);  // skips header line
    while (ifs.good() && std::getline(ifs, line))
    {
        Item product { parse_line(line) };
        products.push_back(product);
    }

    return { boxes, products };
}

bool is_covering(const int N, const int t_numBoxes, const std::vector<int>& t_positions, const std::vector<int>& t_fm)
{
    bool result { true };

    for (int i {}; i < N; ++i)
    {
        result &= std::any_of(
                      cbegin(t_fm)+t_positions[i]*t_numBoxes, 
                      cbegin(t_fm)+(t_positions[i]+1)*t_numBoxes, 
                      [](const int x) { return x != 0; });
    }

    return result;
}

double calculate_price(const std::vector<int>& t_positions, const std::vector<float>& t_prices)
{
    float result {};
    for (int i : t_positions) 
    {
        result += t_prices[i];
    }

    return result;
}

/*
 *
 */
void compute(const int N, const int t_numBoxes, const int t_startIdx, const std::vector<float>& t_prices, const std::vector<int>& t_fm)
{
    std::vector<int> positions;
    positions.reserve(N);
    positions[0] = t_startIdx;

    int i { 0 };
    while (t_startIdx + i < t_fm.size() - N + 2)
    {
        for (int j { 1 }; j < N; ++j) 
        {
            positions[j] = t_startIdx+i+j;
        }

        while (positions.back() < t_numBoxes) 
        {
            if (is_covering(N, t_numBoxes, positions, t_fm))
            {
                calculate_price(positions, t_prices);
            }
            ++positions.back();
        }

        ++i;
    }
}

int main(int argc, char* argv[])
{
    if (argc % 2 == 1)
    {
        return 1;
    }

    int N { 10 };
    std::string product_path {}, box_path { "./data/boxes.csv" };
    for (int i {}; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-N")
        {
            N = std::stoi(argv[++i]);
        } 
        else if (arg == "--boxes" && std::string{ argv[i+1] }.ends_with(".csv"sv))
        {
            
        } 
        else if (arg == "--products" && std::string{ argv[i+1] }.ends_with(".csv"sv))
        {

        }
    }

    auto [boxes, products] { parse_csv(box_path, product_path) };
    auto fm { construct_feasibility_matrix(boxes, products) };

    std::vector<float> prices; 
    prices.reserve(boxes.size());
    std::transform(begin(boxes), end(boxes), begin(prices), 
            [](const Item& item) { return item.price; });

    compute(N, boxes.size(), 0, prices, fm);
    
    // threads here
    // for (int i {}; i < boxes.size() - N; ++i)
    // {
    // }
    // compute();
}
