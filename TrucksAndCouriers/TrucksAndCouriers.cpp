// TrucksAndCouriers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <numeric>
#include <algorithm>

using namespace std;

ofstream outfile( "out.txt" );
ofstream logfile( "log.txt" );

int getWhKey( int x, int y )
{
    return x * 1000 + y;
}

int getZoneKey( int x, int y )
{
    int x_ = x / 200;
    int y_ = y / 200;
    return x_ * 5 + y_;
}

std::vector< int > sort_by_keys( std::map< int, int > & x )
{
    std::vector< int > k;
    for ( auto & p : x )
        k.push_back( p.first );

    // sort keys based on values in v
    std::sort( k.begin(), k.end(),
        [&x]( int i1, int i2 ) { return x[ i1 ] > x[ i2 ]; } );

    return k;
}

template< typename T >
std::vector< size_t > sort_indexes( const std::vector< T > & v )
{
    // initialize original index locations
    std::vector< size_t > idx( v.size() );
    std::iota( idx.begin(), idx.end(), 0 );

    // sort indexes based on comparing values in v
    std::sort( idx.begin(), idx.end(),
        [&v]( size_t i1, size_t i2 ) { return v[ i1 ] < v[ i2 ]; } );

    return idx;
}

struct Customer
{
    Customer() : x{ 0 }, y{ 0 }, it{ -1 } {};
    Customer( int x_, int y_, int it_ ) : x{ x_ }, y{ y_ }, it{ it_ } {};
    int x;
    int y;
    int it;
};

struct TruckDelivery
{
    int x;
    int y;
    int cost;
    vector< Customer > customers;
};

class ItemRecord
{
public:
    ItemRecord() {};
    void addWhsInfo( int whsKey, int quantity )
    {
        whs[ whsKey ] += quantity;
    }

    //void addCustomerInfo( int x, int y )
    //{
    //    cx.push_back( x );
    //    cy.push_back( y );
    //}

public:
    map< int, int > whs;
    //vector< int > cx;
    //vector< int > cy;
};

class ItemMap
{
public:
    ItemMap() {};
    void addItems( int x, int y, int item, int q )
    {

        int k = getWhKey( x, y );
        items[ item ].addWhsInfo( k, q );
    }

    //void addCustomer( int x, int y, int item )
    //{
    //    items[ item ].addCustomerInfo( x, y );
    //}

    ItemRecord& operator[]( int i )
    {
        return items[ i ];
    }

public:
    map< int, ItemRecord > items;
};

class Warehouse
{
public:
    Warehouse() {};
    Warehouse( int x, int y ) : x { x }, y { y } {};
    void addItems( int item, int q )
    {
        items[ item ] += q;
    }

    bool has( int it )
    {
        bool ret = false;
        if ( items.count( it ) > 0 )
            if ( items[ it ] > 0 )
                ret = true;

        return ret;
    }

    int distance( int x_, int y_ )
    {
        return abs( x_ - x ) + abs( y - y_ );
    }

public:
    int x;
    int y;
    map< int, int > items;
};

class WarehouseMap
{
public:
    WarehouseMap() {};
    void addItems( int x, int y, int item, int m )
    {
        int k = getWhKey( x, y );
        if ( !whs.count( k ) )
        {
            Warehouse w( x, y );
            whs[ k ] = w;
        }
        whs[ k ].addItems( item, m );
    }

    Warehouse& operator[]( int i )
    {
        return whs[ i ];
    }

public:
    map< int, Warehouse > whs;
};


class Zone
{
public:
    Zone() {};
    void addCustomer( int x, int y, int itm )
    {
        ci[ itm ].push_back( Customer( x, y, itm ) );
        demand[ itm ]++;
    }

public:
    map< int, vector< Customer > > ci;
    map< int, int > demand;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TrucksAndCouriers
{
public:
    TrucksAndCouriers() :
        zones( 25, Zone() )
    {
    }

    void createWearhouseItemRecord( vector< int > warehouseX, vector< int > warehouseY,
        vector< int > warehouseItem, vector< int > warehouseQuantity )
    {
        size_t n = warehouseX.size();
        for ( size_t i = 0; i < n; ++i )
        {
            whmap.addItems( warehouseX[ i ], warehouseY[ i ], warehouseItem[ i ], warehouseQuantity[ i ] );
            itmap.addItems( warehouseX[ i ], warehouseY[ i ], warehouseItem[ i ], warehouseQuantity[ i ] );
        }
    }

    void createItemCustomerRecord( vector< int > customerX, vector< int > customerY, vector< int > customerItem )
    {
        size_t n = customerX.size();
        for ( size_t i = 0; i < n; ++i )
        {
            //itmap.addCustomer( customerX[ i ], customerY[ i ], customerItem[ i ] );
            int k = getZoneKey( customerX[ i ], customerY[ i ] );
            zones[ k ].addCustomer( customerX[ i ], customerY[ i ], customerItem[ i ] );
        }
    }

    TruckDelivery groupCustomer( map< int, int > truck, Zone & z )
    {
        vector< Customer > group;
        Customer group_center;
        int n = 0;

        for ( auto & it : truck )
            for ( auto & c : z.ci[ it.first ] )
            {
                group.push_back( c );
                group_center.x += c.x;
                group_center.y += c.y;
                n++;
            }

        group_center.x /= n;
        group_center.y /= n;

        TruckDelivery delivery;
        delivery.x = group_center.x;
        delivery.y = group_center.y;

        vector< int > distances;

        for ( auto & c : group )
            distances.push_back( abs( c.x - group_center.x ) + abs( c.y - group_center.y ) );

        vector< size_t > idxs = sort_indexes( distances );
        vector< int > final_items;
        int truck_cost = 0;

        for ( auto & i : idxs )
        {
            Customer c = group[ i ];
            if ( truck[ c.it ] > 0 )
            {
                delivery.customers.push_back( c );
                truck[ c.it ]--;
                truck_cost += distances[ i ];
            }
        }

        delivery.cost = truck_cost;

        return delivery;
    }

    void determineSupply( Zone & z )
    {
        map< int, int > supp_stat;
        map< int, map< int, int > > supp;

        for ( auto & d : z.demand )
        {
            for ( auto & r : itmap[ d.first ].whs )
            {
                int q = d.second < r.second ? d.second : r.second;
                supp_stat[ r.first ] += q;
                supp[ r.first ][ d.first ] = q;
            }
        }

        vector< int > k = sort_by_keys( supp_stat );
        int whkey = k[ 0 ];
        TruckDelivery delivery = groupCustomer( supp[ whkey ], z );
        delivery.cost += ( tfixed + tvariable * ( abs( delivery.x - whmap[ whkey ].x ) + abs( delivery.y - whmap[ whkey ].y ) ) );

        map< int, ItemRecord > group_itmap;

        for ( auto & c : delivery.customers )
            group_itmap[ c.it ] = itmap[ c.it ];

        int gready_cost = 0;

        for ( auto & c : delivery.customers )
        {
            int d = 2000;
            int k = -1;
            for ( auto & wh : group_itmap[ c.it ].whs )
            {
                if ( wh.second > 0 )
                {
                    int d_ = abs( c.x - whmap[ wh.first ].x ) + abs( c.y - whmap[ wh.first ].y );
                    if ( d_ < d )
                    {
                        d = d_;
                        k = wh.first;
                    }
                }
            }

            if ( k != -1 )
            {
                group_itmap[ c.it ].whs[ k ]--;
                gready_cost += d;
            }
        }

        int stop = 9;
    }

    void determineZoneSupplies()
    {
        for ( auto & z : zones )
            determineSupply( z );
    }

    vector< string > planShipping( int truckFixed, int truckVariable, vector< int > warehouseX,
        vector< int > warehouseY, vector< int > warehouseItem, vector< int > warehouseQuantity,
        vector< int > customerX, vector< int > customerY, vector< int > customerItem )
    {
        tfixed = truckFixed;
        tvariable = truckVariable;
        createWearhouseItemRecord( warehouseX, warehouseY, warehouseItem, warehouseQuantity );
        createItemCustomerRecord( customerX, customerY, customerItem );


        determineZoneSupplies();

        return vector< string >();
    }

private:
    WarehouseMap whmap;
    ItemMap itmap;
    vector< Zone > zones;
    int tfixed;
    int tvariable;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
ostream& operator<<( ostream & os, vector< T > & v )
{
    os << v.size() << endl;
    for ( auto & x : v )
        os << x << " ";
    os << endl;

    return os;
}

template< typename T >
istream& operator>>( istream & is, vector< T > & v )
{
    int n;
    is >> n;
    for ( int i = 0; i < n; ++i )
    {
        T x;
        is >> x;
        v.push_back( x );
    }

    return is;
}

void runTest( istream & is, bool writeToLog = false )
{
    int truckFixed;
    is >> truckFixed;

    int truckVariable;
    is >> truckVariable;

    vector< int > warehouseX;
    is >> warehouseX;

    vector< int > warehouseY;
    is >> warehouseY;

    vector< int > warehouseItem;
    is >> warehouseItem;

    vector< int > warehouseQuantity;
    is >> warehouseQuantity;

    vector< int > customerX;
    is >> customerX;

    vector< int > customerY;
    is >> customerY;

    vector< int > customerItem;
    is >> customerItem;

    if ( writeToLog )
    {
        outfile << truckFixed << endl;
        outfile << truckVariable << endl;
        outfile << warehouseX;
        outfile << warehouseY;
        outfile << warehouseItem;
        outfile << warehouseQuantity;
        outfile << customerX;
        outfile << customerY;
        outfile << customerItem;
    }

    TrucksAndCouriers sol;
    vector< string > ret = sol.planShipping( truckFixed, truckVariable, warehouseX, warehouseY,
        warehouseItem, warehouseQuantity, customerX, customerY, customerItem );

    cout << ret.size();
    for ( auto & s : ret )
        cout << s << "\n";

}

int main()
{
    //runTest( cin, true );

    ifstream infile( "in.txt" );
    runTest( infile, true );
}


