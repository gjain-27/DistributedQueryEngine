CREATE TABLE customers (
    customer_id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    country_code char(3) NOT NULL,
    created_at timestamptz NOT NULL DEFAULT current_timestamp
);

CREATE TABLE products (
    product_id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    category varchar(100) NOT NULL,
    base_price bigint NOT NULL DEFAULT 0,
    currency_code char(3) NOT NULL
);

CREATE TABLE stores (
    store_id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    country_code char(3) NOT NULL
);

CREATE TABLE transactions (
    transaction_id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    customer_id uuid NOT NULL,
    product_id uuid NOT NULL,
    store_id uuid NOT NULL,
    currency_code char(3) NOT NULL,
    quantity integer NOT NULL,
    total_amount BIGINT NOT NULL,
    occurred_at timestamptz NOT NULL DEFAULT current_timestamp,

    FOREIGN KEY (customer_id) REFERENCES customers(customer_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (store_id) REFERENCES stores(store_id)
);