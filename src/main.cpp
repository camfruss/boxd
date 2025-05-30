
/*
- keep track of location ... use long long so can restart if stopped
- brute force w/ small search space w/ threads
- generate subsets of size N (via indices ... use unsigned short)
- take any(item & subset)    TAKE | of all columns
     - if 0: terminate
- take MSB index, price++
- sum all prices
- keep priority queue of min 1000 prices
- std::array<uint8_fast_t, N> combination; float cost;

feasibility_matrix
return std::vector<float> prices
*/
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


using uint8 = uint_fast8_t;

struct Item
{
    std::string id;
    std::array<uint8, 3> dims;
    float price {};
};

void construct_feasibility_matrix(const std::vector<Item>& t_boxes, const std::vector<Item> t_products)
{
    // also need to read in items ...
}

Item parse_line(const std::string& t_line, bool t_hasPrice = false)
{
    Item item {};
    std::istringstream iss { t_line };
    std::string tmp {};  
    
    std::getline(iss, item.id, ',');

    std::getline(iss, tmp, ',');  item.dims[0] = static_cast<uint8>(std::stof(tmp)); // dim_1
    std::getline(iss, tmp, ',');  item.dims[1] = static_cast<uint8>(std::stof(tmp)); // dim_2
    std::getline(iss, tmp, ',');  item.dims[2] = static_cast<uint8>(std::stof(tmp)); // dim_3

    if (t_hasPrice)
    {
        std::getline(iss, tmp, ',');
        item.price = std::stof(tmp);
    }

    return item;
}

/* Requires columns be id, dim1, dim2, dim3, price?, ... */
void parse_csv(const std::string& t_boxPath, const std::string& t_productPath)
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
}

void generate_subsets(const uint_fast16_t N)
{
   // given range of [0, N-1], generate subsets of size N 
   // starting index
}

bool is_covering()
{
    return false;
}

void calculate_price()
{

}

int main(int argc, char* argv[])
{
    std::string box_path { argv[1] }; // first argument
    std::string product_path { argv[2] }; // second argument

    parse_csv(box_path, product_path);
}
