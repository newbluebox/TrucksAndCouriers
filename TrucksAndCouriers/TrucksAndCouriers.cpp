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
#include <random>
#include <sstream>
#include <limits>


using namespace std;


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
    Customer() : x { 0 }, y { 0 }, it { -1 } {};
    Customer( int x_, int y_, int it_ ) : x{ x_ }, y{ y_ }, it{ it_ } {};
    int x;
    int y;
    int it;
};

struct TruckDelivery
{
    int x;
    int y;
    int wx;
    int wy;
    int cost;
    int r;
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

    int& operator[] ( int i )
    {
        return whs[ i ];
    }

public:
    // warehouse key vs quantity
    map< int, int > whs;
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

    ItemRecord& operator[]( int i )
    {
        return items[ i ];
    }

private:
    // by item
    map< int, ItemRecord > items;
};

//class CustomerRecord
//{
//public:
//    CustomerRecord() {};
//    void addCustomer( Customer & c )
//    {
//        int k = getWhKey( c.x, c.y );
//        customers
//    }
//public:
//    map< int, Customer > customers;
//};

class CustomerMap
{
public:
    CustomerMap() {};
    void addCustomer( Customer & c )
    {
        customers[ c.it ].push_back( c );
    }

    void addCustomer( int x, int y, int it )
    {
        customers[ it ].push_back( Customer( x, y, it ) );
    }

    vector< Customer >& operator[]( int i )
    {
        return customers[ i ];
    }
private:
    // by item
    map< int, vector< Customer > > customers;
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

    int distance( int x_, int y_ )
    {
        return abs( x_ - x ) + abs( y - y_ );
    }

    int& operator[] ( int i )
    {
        return items[ i ];
    }

    map< int, int >::iterator begin()
    {
        return items.begin();
    }

    map< int, int >::iterator end()
    {
        return items.end();
    }

    map< int, int > getMap()
    {
        return items;
    }

private:
    // itme vs quantity
    map< int, int > items;

public:
    int x;
    int y;
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

    map< int, Warehouse >::iterator begin()
    {
        return whs.begin();
    }

    map< int, Warehouse >::iterator end()
    {
        return whs.end();
    }


private:
    // by warehouse key
    map< int, Warehouse > whs;
};

class blob
{
public:
    blob() {};
    blob( int x_, int y_, int r_ ) : x { x_ }, y { y_ }, r { r_ }
    {};
public:
    int x;
    int y;
    int r;
};


class Zone
{
public:
    Zone() {};
    void addCustomer( Customer & c )
    {
        ci[ c.it ].push_back( c );
        demand[ c.it ]++;
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
        m_gen( 42 ), m_point_dist( 0, 1000 ), m_dis_1( 0, 1 )
    {
    }

    void createRecords( vector< int > warehouseX, vector< int > warehouseY,
        vector< int > warehouseItem, vector< int > warehouseQuantity, vector< int > customerX, vector< int > customerY, vector< int > customerItem )
    {
        size_t n = warehouseX.size();
        for ( size_t i = 0; i < n; ++i )
        {
            whmap.addItems( warehouseX[ i ], warehouseY[ i ], warehouseItem[ i ], warehouseQuantity[ i ] );
            itmap.addItems( warehouseX[ i ], warehouseY[ i ], warehouseItem[ i ], warehouseQuantity[ i ] );
        }

        n = customerX.size();
        for ( size_t i = 0; i < n; ++i )
            cmap.addCustomer( customerX[ i ], customerY[ i ], customerItem[ i ] );
    }

    /*void createItemCustomerRecord( vector< int > customerX, vector< int > customerY, vector< int > customerItem )
    {
        size_t n = customerX.size();
        for ( size_t i = 0; i < n; ++i )
        {
            //customers.push_back( Customer( customerX[ i ], customerY[ i ], customerItem[ i ] ) );
            //customer_item_map[ getWhKey( customerX[ i ], customerY[ i ] ) ][ customerItem[ i ] ]++;
            cmap.addCustomer( customerX[ i ], customerY[ i ], customerItem[ i ] );
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

    int greadyCost( vector< Customer > & customers )
    {
        map< int, ItemRecord > group_itmap;

        for ( auto & c : customers )
            group_itmap[ c.it ] = itmap[ c.it ];

        int gready_cost = 0;

        for ( auto & c : customers )
        {
            int d = 2000;
            int k = -1;
            for ( auto & wh : group_itmap[ c.it ].whs )
            {
                if ( wh.second > 0 )
                {
                    int d_ = whmap[ wh.first ].distance( c.x, c.y );
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

        return gready_cost;
    }

    TruckDelivery createTruckDelivery( Zone & z )
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
        vector< TruckDelivery > deliveries;
        int j = -1;
        double thrhld = 1.2;
        double best_ratio = thrhld;
        int K = k.size() < 5 ? k.size() : 5;
        for ( int i = 0; i < K; ++i )
        {
            int whkey = k[ i ];
            TruckDelivery d = groupCustomer( supp[ whkey ], z );
            d.cost += ( tfixed + tvariable * ( whmap[ whkey ].distance( d.x, d.y ) ) );
            d.wx = whmap[ whkey ].x;
            d.wy = whmap[ whkey ].y;

            d.gready_cost = greadyCost( d.customers );

            double ratio = d.gready_cost / ( (double)d.cost );
            if ( ratio > thrhld )
                deliveries.push_back( d );

            if ( ratio > best_ratio )
            {
                best_ratio = ratio;
                j = deliveries.size() - 1;
            }
        }

        if ( j == -1 )
            return TruckDelivery();

        return deliveries[ j ];
    }

    Zone randomZone( int sz )
    {
        int x = m_point_dist( m_gen );
        int y = m_point_dist( m_gen );

        int x_min = 0;
        int y_min = 0;
        int x_max = 1000;
        int y_max = 1000;

        int d = sz / 2;

        if ( x - d >= 0 && x + d <= 1000 )
        {
            x_min = x - d;
            x_max = x + d;
        }
        else if ( x - d >= 0 )
        {
            x_min = 1000 - sz;
        }
        else if ( x + d <= 1000 )
        {
            x_max = sz;
        }

        if ( y - d >= 0 && y + d <= 1000 )
        {
            y_min = y - d;
            y_max = y + d;
        }
        else if ( y - d >= 0 )
        {
            y_min = 1000 - sz;
        }
        else if ( y + d <= 1000 )
        {
            y_max = sz;
        }

        Zone z;
        for ( auto & c : customers )
            if ( c.x >= x_min && c.x <= x_max && c.y >= y_min && c.y <= y_max 
                && customer_item_map[ getWhKey( c.x, c.y ) ][ c.it ] > 0 )
                z.addCustomer( c );

        return z;
    }


    void makeTruckDelivery( TruckDelivery & d )
    {
        stringstream ss;
        ss << "T," << d.wx << "," << d.wy << "," << d.x << "," << d.y;
        int whkey = getWhKey( d.wx, d.wy );
        int dest_whkey = getWhKey( d.x, d.y );

        for ( auto & c : d.customers )
        {
            ss << "," << c.it;
            whmap[ whkey ][ c.it ]--;
            //whmap.addItems( d.x, d.y, c.it, 1 );
            itmap[ c.it ][ whkey ]--;
            //itmap[ c.it ][ dest_whkey ]++;
        }

        delivery_log.push_back( ss.str() );

        for ( auto & c : d.customers )
        {
            ss.clear();
            ss.str( "" );
            ss << "C," << d.x << "," << d.y << "," << c.x << "," << c.y << "," << c.it;
            customer_item_map[ getWhKey( c.x, c.y ) ][ c.it ]--;
            delivery_log.push_back( ss.str() );
        }
    }

    void makeGreadyCourierDeliveries()
    {
        stringstream ss;
        for ( auto & c : customers )
        {
            if ( customer_item_map[ getWhKey( c.x, c.y ) ][ c.it ] > 0 )
            {
                int d = 2000;
                int k = -1;
                for ( auto & wh : itmap[ c.it ].whs )
                {
                    if ( wh.second > 0 )
                    {
                        int d_ = whmap[ wh.first ].distance( c.x, c.y );
                        if ( d_ < d )
                        {
                            d = d_;
                            k = wh.first;
                        }
                    }
                }

                if ( k != -1 )
                {
                    ss.clear();
                    ss.str( "" );
                    ss << "C," << whmap[ k ].x << "," << whmap[ k ].y << "," << c.x << "," << c.y << "," << c.it;
                    delivery_log.push_back( ss.str() );
                    customer_item_map[ getWhKey( c.x, c.y ) ][ c.it ]--;
                    itmap[ c.it ].whs[ k ]--;
                }
            }
        }
    }*/

    TruckDelivery createTruckDelivery( Warehouse & wh )
    {
        vector< Customer > potential_customers;
        for ( auto & iq : wh )
            potential_customers.insert( potential_customers.end(), cmap[ iq.first ].begin(), cmap[ iq.first ].end() );

        int x_min = potential_customers[ 0 ].x;
        int x_max = potential_customers[ 0 ].x;
        int y_min = potential_customers[ 0 ].y;
        int y_max = potential_customers[ 0 ].y;

        for ( auto & c : potential_customers )
        {
            if ( c.x < x_min )
                x_min = c.x;

            if ( c.x > x_max )
                x_max = c.x;

            if ( c.y < y_min )
                y_min = c.y;

            if ( c.y > y_max )
                y_max = c.y;
        }

        blob b( ( x_min + x_max ) / 2, ( y_min + y_max ) / 2, 100 );
        double cost = numeric_limits<float>::max();
        std::uniform_int_distribution<> step_dist( 0, 2 );

        double cost_thresh = 500;

        int noChagneThresh = 100;
        int changeind = 0;

        vector< size_t > blob_idxs;

        while ( changeind < noChagneThresh )
        {
            blob new_b = b;
            int dx = ( step_dist( m_gen ) - 1 ) * 20;
            if ( ( new_b.x + dx ) >= x_min && ( new_b.x + dx ) < x_max )
                new_b.x += dx;
            
            int dy = ( step_dist( m_gen ) - 1 ) * 20;
            if ( ( new_b.y + dy ) >= y_min && ( new_b.y + dy ) < y_max )
                new_b.y += dy;

            int dr = ( step_dist( m_gen ) - 1 ) * 10;
            if ( ( new_b.r + dr ) >= 50 && ( new_b.r + dr ) < 250 )
                new_b.r += dr;

            vector< int > distances;
            for ( auto & c : potential_customers )
                distances.push_back( abs( new_b.x- c.x ) + abs( new_b.y - c.y ) );

            vector< size_t > idxs = sort_indexes( distances );
            map< int, int > supply = wh.getMap();
            double new_cost = tfixed + tvariable * wh.distance( new_b.x, new_b.y );
            int n = 0;
            vector< size_t > new_blob_idxs;
            for ( auto & i : idxs )
            {
                if ( distances[ i ] < new_b.r && supply[ potential_customers[ i ].it ] > 0 )
                {
                    new_cost += distances[ i ];
                    supply[ potential_customers[ i ].it ]--;
                    new_blob_idxs.push_back( i );
                    n++;
                }
            }

            new_cost = new_cost / n;

            double delta = new_cost - cost;

            if ( delta < cost_thresh )
            {
                b = new_b;
                cost = new_cost;
                blob_idxs = new_blob_idxs;
                if ( delta < 0 )
                {
                    cost_thresh *= 0.5;
                    changeind = 0;
                }
            }
            changeind++;
        }

        TruckDelivery delivery;
        delivery.x = b.x;
        delivery.y = b.y;
        delivery.wx = wh.x;
        delivery.wy = wh.y;
        delivery.r = b.r;
        delivery.cost = cost;
        for ( auto & i : blob_idxs )
            delivery.customers.push_back( potential_customers[ i ] );

        return delivery;
    }


    vector< string > planShipping( int truckFixed, int truckVariable, vector< int > warehouseX,
        vector< int > warehouseY, vector< int > warehouseItem, vector< int > warehouseQuantity,
        vector< int > customerX, vector< int > customerY, vector< int > customerItem )
    {
        tfixed = truckFixed;
        tvariable = truckVariable;
        createRecords( warehouseX, warehouseY, warehouseItem, warehouseQuantity, customerX, customerY, customerItem );

        vector< TruckDelivery > tds;
        for ( auto & wh : whmap )
            tds.push_back( createTruckDelivery( wh.second ) );

        return delivery_log;
    }

private:
    WarehouseMap whmap;
    ItemMap itmap;
    CustomerMap cmap;
    //vector< Customer > customers;
    int tfixed;
    int tvariable;

    mt19937 m_gen;
    std::uniform_int_distribution<> m_point_dist;
    std::uniform_real_distribution<> m_dis_1;
    vector< string > delivery_log;

    //map< int, map< int, int > > customer_item_map;

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ofstream outfile( "out.txt" );
ofstream logfile( "log.txt" );

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

    cout << ret.size() << endl;

    if ( writeToLog )
        logfile << ret.size() << endl;

    for ( auto & s : ret )
    {
        if ( writeToLog )
            logfile << s << endl;

        cout << s << endl;
    }

    cout.flush();

    if ( writeToLog )
    {
        logfile << "Finished";
        logfile.flush();
    }
}


int main()
{
    //runTest( cin, true );

    ifstream infile( "in.txt" );
    runTest( infile, false );
}


