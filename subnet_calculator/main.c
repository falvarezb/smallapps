#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

/**
 * @brief data structure to represent an IP address in dotted decimal notation
 * 
 * e.g. 192.168.1.0 == {192, 168, 1, 0}
 */
typedef struct {
    u_int8_t byte1;
    u_int8_t byte2;
    u_int8_t byte3;
    u_int8_t byte4;
} ip_address_t;

/**
 * @brief data structure to represent a network
 * 
 * e.g.
 * IP address 9.9.8.2/23 belongs to network:
 * 
 * network address: 9.9.8.0
 * broadcast address: 9.9.9.255
 * first address: 9.9.8.1
 * last address: 9.9.9.254
 * next network: 9.9.10.0
 * subnet mask: 255.255.254.0
 * number of addresses: 512
 * prefix length: 23
 */
typedef struct {
    uint32_t network_address;
    uint32_t broadcast_address;
    uint32_t first_address;
    uint32_t last_address;
    uint32_t next_network;
    uint32_t subnet_mask;    
    uint32_t num_ip_addresses;
    int prefixlen;
} subnet_t;



ip_address_t to_dotted_decimal_notation(uint32_t ip_address) {
    return (ip_address_t) {ip_address >> 24 & 0xFF, ip_address >> 16 & 0xFF, ip_address >> 8 & 0xFF, ip_address & 0xFF};
}

uint32_t to_int(ip_address_t ip_address) {
    return (ip_address.byte1 << 24) + (ip_address.byte2 << 16) + (ip_address.byte3 << 8) + ip_address.byte4;
}

void print_formatted_ip_address(uint32_t ip_address, const char* label) {
    ip_address_t decimal_notation = to_dotted_decimal_notation(ip_address);    
    printf("%s: %u.%u.%u.%u\n", label, decimal_notation.byte1, decimal_notation.byte2, decimal_notation.byte3, decimal_notation.byte4);
}

void print_subnet_params(subnet_t subnet_params) {
    print_formatted_ip_address(subnet_params.network_address, "network address");
    print_formatted_ip_address(subnet_params.broadcast_address, "broadcast address");
    print_formatted_ip_address(subnet_params.first_address, "first address");
    print_formatted_ip_address(subnet_params.last_address, "last address");
    print_formatted_ip_address(subnet_params.next_network, "next network");
    print_formatted_ip_address(subnet_params.subnet_mask, "subnet mask");
    printf("%s: %d\n", "prefix length", subnet_params.prefixlen);
    printf("%s: %d\n", "number of addresses", subnet_params.num_ip_addresses);
}

/**
 * @brief Calculate subnet parameters for a given ip address and cidr
 * 
 * @param ip_address 
 * @param cidr_prefix 
 * @return subnet_t 
 */
subnet_t subnet_calculator(uint32_t ip_address, int cidr_prefix) {
    //print_formatted_ip_address(ip_address, "ip address");

    subnet_t subnet_params;   

    // calculate the network address by setting to 0 the host bits of the ip address
    subnet_params.network_address = ip_address & (0xFFFFFFFF << (32 - cidr_prefix));
    // calculate the broadcast address by setting to 1 the host bits of the ip address
    subnet_params.broadcast_address = ip_address | (0xFFFFFFFF >> cidr_prefix);
    subnet_params.first_address = subnet_params.network_address + 1;    
    subnet_params.last_address = subnet_params.broadcast_address - 1;    
    subnet_params.next_network = subnet_params.broadcast_address + 1;    
    // calculate the subnet mask by setting to 1 the network bits of the network address
    subnet_params.subnet_mask = subnet_params.network_address | (0xFFFFFFFF << (32 - cidr_prefix));    
    subnet_params.num_ip_addresses = subnet_params.broadcast_address - subnet_params.network_address + 1; 
    subnet_params.prefixlen = cidr_prefix;   
    return subnet_params;
}

/**
 * @brief Given a subnet of size /n, calculate the maximum subnet size possible to create 'x' subnets
 * 
 * Example: in a subnet of size /18 it is possible to fit up to 128 subnets of size /25
 * 
 * @param original_subnet_size 
 * @param num_subnets 
 * @return int maximum size of the subnets
 */
int calculate_subnet_size(int original_subnet_size, int num_subnets) {
    //each subdivision splits the original subnet into 2 new subnets and each subdivision equates to increasing
    //the cidr by 1
    int num_subdivisions = log2(num_subnets);
    return original_subnet_size + num_subdivisions + 1;
}

/**
 * @brief Given a subnet of size /n, calculate how many subnets can be created that contain at least 'x' ip addresses
 * 
 * Example: in a subnet of size /21 it is possible to create 32 subnets that contain at least 50 ip addresses
 * 
 * It's the inverse function of 'calculate_subnet_size'
 * 
 * @param original_subnet_size 
 * @param num_ip_addresses 
 * @return int maximum number of subnets
 */
int calculate_num_subnets(int original_subnet_size, int num_ip_addresses) {
    //num bits required to represent num_ip_addresses
    int num_bits = log2(num_ip_addresses) + 1;
    //remaining bits to use for the new subnets
    int available_bits = 32 - num_bits - original_subnet_size;
    //num subnets that can be created with the available bits
    int num_subnets = exp2(available_bits);
    return num_subnets;
}

/**
 * @brief Calculate the smallest subnet that can contain 'x' hosts
 * 
 * e.g. the smallest subnet that can contain 10 hosts is /28
 *  
 * @param num_ip_addresses 
 * @return int 
 */
int calculate_subnet_prefixlen(int num_ip_addresses) {
    //factor to account for the existence of network and broadcast addresses and therefore:
    //usable addresses = 2^(num bits) - 2
    //in theory it should be 2, but given that we are doing log2 + 1, a value of 1 suffices
    int usable_ip_addresses_factor = 1;
    int num_bits = log2(num_ip_addresses + usable_ip_addresses_factor) + 1;
    return 32 - num_bits;
}

int compare_func(const void* a, const void* b) {
    return ((subnet_t*)a)->prefixlen - ((subnet_t*)b)->prefixlen;
}

/**
 * @brief Calculate Variable-Length Subnet Masks
 * 
 * The desired subnets are passed as an array of objects subnet_t, specifying the minimum number of hosts of 
 * each subnet with the attribute 'num_ip_addresses'
 * 
 * The function updates each object with the corresponding parameters of the subnet. The elements of the array are sorted
 * according to the size of the subnet in descending order.
 * The original value of the attribute 'num_ip_addresses' is overwritten with the total number of ip addresses in the subnet
 * 
 * @param original_subnet 
 * @param subnets in-out parameter
 * @param num_subnets 
 * @return subnet_t* For convenience, the modified parameter 'subnets` is also returned
 */
subnet_t* vlsm(subnet_t* original_subnet, subnet_t subnets[], size_t num_subnets) {

    //sanity check
    uint32_t total_num_ip_address_required = 0;
    for (size_t i = 0; i < num_subnets; i++){
        total_num_ip_address_required += calculate_subnet_prefixlen(subnets[i].num_ip_addresses);
    }
    uint32_t total_num_ip_address_available = exp2(32 - original_subnet->prefixlen);
    assert(total_num_ip_address_required < total_num_ip_address_available);

    //calculate minimum subnet size    
    for (size_t i = 0; i < num_subnets; i++){
        subnets[i].prefixlen = calculate_subnet_prefixlen(subnets[i].num_ip_addresses);
    }

    //sort subnets by size in descending order (ascending order by prefix length)
    qsort(subnets, num_subnets, sizeof(subnet_t), compare_func);

    //allocate subnets
    subnets[0] = subnet_calculator(original_subnet->network_address, subnets[0].prefixlen);    
    for (size_t i = 1; i < num_subnets; i++){        
        subnets[i] = subnet_calculator(subnets[i-1].next_network, subnets[i].prefixlen);
    }

    return subnets;
}

void vlsm_test_cases() {
    {
        subnet_t original_subnet = {.network_address = 151587072, .prefixlen = 24};
        size_t num_subnets = 3;
        subnet_t target_subnets[] = {{.num_ip_addresses=25}, {.num_ip_addresses=50}, {.num_ip_addresses=10}};
        vlsm(&original_subnet, target_subnets, num_subnets);
        for (size_t i = 0; i < num_subnets; i++){
            printf("\n");
            print_subnet_params(target_subnets[i]);
        }    
    }

    {
        subnet_t original_subnet = {.network_address = 151587072, .prefixlen = 23};
        size_t num_subnets = 3;
        subnet_t target_subnets[] = {{.num_ip_addresses=25}, {.num_ip_addresses=63}, {.num_ip_addresses=10}};
        vlsm(&original_subnet, target_subnets, num_subnets);
        for (size_t i = 0; i < num_subnets; i++){
            printf("\n");
            print_subnet_params(target_subnets[i]);
        }    
    }
}

int main(int argc, char const *argv[])
{
    print_subnet_params(subnet_calculator(151587072, 23));
    // printf("subnet /%d needs to be split into subnets of size /%d to have at least %d subnets\n", 18, calculate_subnet_size(18, 100), 100);
    // printf("in a subnet /%d it is possible to create %d subnets that contain at least %d ip addresses\n", 21, calculate_num_subnets(21, 50), 50);
    // printf("%d\n", to_int((ip_address_t){9,9,9,0}));

    //vlsm_test_cases();

    return 0;
}
